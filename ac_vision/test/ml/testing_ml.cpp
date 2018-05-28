#include "../../src/utils/FileUtils.h"
#include "../../src/utils/StringUtils.h"

#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;
using namespace std;

string feature_type;
string ml_type;
DenseFeatureDetector detector ( 1, 1, 1, 8, 3 );
DenseFeatureDetector freak_detector ( 1, 1, 1, 8, 3 );
DenseFeatureDetector brief_detector ( 1, 1, 1, 8, 3 );
int nfeatures = 64;
int descriptor_length;

/// classifiers
CvSVM svm;
CvANN_MLP ann_mlp;
CvKNearest knn;
EM em ( EM::DEFAULT_NCLUSTERS, EM::COV_MAT_DIAGONAL, TermCriteria ( CV_TERMCRIT_ITER, ( int ) 1e7, 1e-6 ) );
CvERTrees er_trees;
CvRTrees r_trees;
CvGBTrees gb_trees;
CvBoost boost;
CvDTree d_trees;
CvNormalBayesClassifier nbc;

/// brief
int bytes = 64;
BriefDescriptorExtractor brief_extractor ( bytes );

/// brisk
int thresh = 30;
int octaves = 3;
float pattern_scale = 1.0f;
BRISK brisk_extractor ( thresh, octaves, pattern_scale );

/// cascade
CascadeClassifier cascade_classifier;

/// freak
bool orientation_normalised = true;
bool scale_normalised = true;
float freak_pattern_scale = 22.0f;
int noctaves = 4;
vector< int > selected_pairs;
FREAK freak_extractor ( orientation_normalised, scale_normalised, freak_pattern_scale, noctaves, selected_pairs );

/// hog
Size win_size ( 64, 64 );
Size block_size ( 16, 16 );
Size block_stride ( 8, 8 );
Size cell_size ( 8, 8 );
int nbins = 9;
int deriv_aperture = 1;
int win_sigma = -1;
int histogram_norm_type = HOGDescriptor::L2Hys;
double l2hys_threshold = 0.2;
bool gamma_correction = true;
int hog_nlevels = HOGDescriptor::DEFAULT_NLEVELS;
HOGDescriptor hog_extractor ( win_size, block_size, block_stride, cell_size, nbins, deriv_aperture,
                              win_sigma, histogram_norm_type, l2hys_threshold, gamma_correction, hog_nlevels );

/// latent
LatentSvmDetector latent_svm_detector;

/// orb
float scale_factor = 1.2f;
int nlevels = 8;
int orb_edge_threshold = 31;
int first_level = 0;
int wta_k = 2;
int score_type = ORB::HARRIS_SCORE;
int patch_size = 3;
OrbDescriptorExtractor orb_extractor ( nfeatures, scale_factor, nlevels, orb_edge_threshold, first_level, wta_k, score_type, patch_size );

/// sift
int noctave_layers = 3;
double contrast_threshold = 0.04;
double edge_threshold = 10;
double sigma = 1.6;
SiftDescriptorExtractor sift_extractor ( nfeatures, noctave_layers, contrast_threshold, edge_threshold, sigma );

/// surf
double hessian_threshold = 100;
int surf_noctaves = 4;
int surf_noctave_layers = 3;
bool extended = true;
bool upright = false;
SurfDescriptorExtractor surf_extractor ( hessian_threshold, surf_noctaves, surf_noctave_layers, extended, upright );

vector< void ( * ) ( Mat, Mat& ) > tocalls;

void load_latent_svm_detector ( string filename )
{
  vector< string > models_filenames ( 1, filename );
  latent_svm_detector.load ( models_filenames );
  if ( latent_svm_detector.empty ( ) )
    {
      exit ( EXIT_FAILURE );
    }
}

void load_cascade_classifier ( string file_name )
{
  if ( !cascade_classifier.load ( file_name ) )
    {
      exit ( EXIT_FAILURE );
    }
}

void is_person_present_latentsvm ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  vector< LatentSvmDetector::ObjectDetection > detections;
  latent_svm_detector.detect ( image, detections, 0 );
  for ( unsigned int i = 0; i < detections.size ( ); i++ )
    {
      if ( detections[i].score >= -1 )
        {
          resultst.push_back ( detections[i].score );
          resultsf.push_back ( 1.0 );
          return;
        }
    }

  resultst.push_back ( 0.0 );
  resultsf.push_back ( 0.0 );
  return;
}

void is_person_present_cascade ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  Mat gray;
  cvtColor ( image, gray, CV_BGR2GRAY );
  equalizeHist ( gray, gray );

  vector< Rect > detected_humans;
  vector< int > reject_levels;
  vector< double > level_weights;
  cascade_classifier.detectMultiScale ( gray, detected_humans, reject_levels, level_weights, 1.1, 3, CASCADE_FIND_BIGGEST_OBJECT );

  cout << " detected humans " << detected_humans.size();
  cout << " reject_levels " << reject_levels.size();
  cout << " level_weights " << level_weights.size();
  for ( unsigned int i = 0; i < detected_humans.size ( ); i++ )
    {
      resultst.push_back ( 1.0 );
      resultsf.push_back ( 1.0 );
      return;
    }

  resultst.push_back ( 0.0 );
  resultsf.push_back ( 0.0 );
}

void is_person_present_hog ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  vector< float > hog_descriptor;
  hog_extractor.compute ( image, hog_descriptor );
  Mat descriptor ( hog_descriptor );
  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  resultst.push_back ( svm.predict ( descriptor, true ) );
  resultsf.push_back ( svm.predict ( descriptor, false ) );
}

void get_sift_descriptor ( Mat image, Mat& descriptor )
{
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );
  sift_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );
}

void is_person_present_sift ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  Mat descriptor;
  get_sift_descriptor ( image, descriptor );

  if ( ml_type.compare ( "svm" ) == 0 )
    {
      resultst.push_back ( svm.predict ( descriptor, true ) );
      resultsf.push_back ( svm.predict ( descriptor, false ) );
    }
  else if ( ml_type.compare ( "knn" ) == 0 )
    {
      // FIXME: knn has no predict
//       resultst.push_back ( knn.predict ( descriptor, true ) );
//       resultsf.push_back ( knn.predict ( descriptor, false ) );
    }
  else if ( ml_type.compare ( "ann_mlp" ) == 0 )
    {
//       CvMat descriptor_cvmat = descriptor;
//       resultst.push_back ( ann_mlp.predict ( descriptor_cvmat, true ) );
//       resultsf.push_back ( ann_mlp.predict ( descriptor_cvmat, false ) );
    }
  else if ( ml_type.compare ( "em" ) == 0 )
    {
//       resultst.push_back ( em.predict ( descriptor, true ) );
//       resultsf.push_back ( em.predict ( descriptor, false ) );
    }
  else if ( ml_type.compare ( "er_trees" ) == 0 )
    {
      resultst.push_back ( er_trees.predict_prob ( descriptor ) );
      resultsf.push_back ( er_trees.predict ( descriptor ) );
    }
  else if ( ml_type.compare ( "r_trees" ) == 0 )
    {
      resultst.push_back ( r_trees.predict_prob ( descriptor ) );
      resultsf.push_back ( r_trees.predict ( descriptor ) );
    }
  else if ( ml_type.compare ( "gb_trees" ) == 0 )
    {
      double tf_value = gb_trees.predict ( descriptor );
      resultst.push_back ( tf_value );
      resultsf.push_back ( tf_value );
    }
  else if ( ml_type.compare ( "boost" ) == 0 )
    {
      resultst.push_back ( boost.predict ( descriptor, Mat(), Range::all(), false, true ) );
      resultsf.push_back ( boost.predict ( descriptor, Mat(), Range::all(), false, false ) );
    }
  else if ( ml_type.compare ( "d_trees" ) == 0 )
    {
      double tf_value = d_trees.predict ( descriptor )->value;
      resultst.push_back ( tf_value );
      resultsf.push_back ( tf_value );
    }
  else if ( ml_type.compare ( "nbc" ) == 0 )
    {
      double tf_value = nbc.predict ( descriptor );
      resultst.push_back ( tf_value );
      resultsf.push_back ( tf_value );
    }
}

void is_person_present_surf ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  // getting the features
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );

  Mat descriptor;
  surf_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  resultst.push_back ( svm.predict ( descriptor, true ) );
  resultsf.push_back ( svm.predict ( descriptor, false ) );
}

void get_brisk_descriptor ( Mat image, Mat& descriptor )
{
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );

  brisk_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );
}

void is_person_present_brisk ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  Mat descriptor;
  get_brisk_descriptor ( image, descriptor );

  resultst.push_back ( svm.predict ( descriptor, true ) );
  resultsf.push_back ( svm.predict ( descriptor, false ) );
}

void get_brief_descriptor ( Mat image, Mat& descriptor )
{
  vector< KeyPoint > keypoints;
  brief_detector.detect ( image, keypoints );
  brief_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );
}

void is_person_present_brief ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  Mat descriptor;
  get_brief_descriptor ( image, descriptor );

  resultst.push_back ( svm.predict ( descriptor, true ) );
  resultsf.push_back ( svm.predict ( descriptor, false ) );
}

void is_person_present_multiple ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  vector< Mat > all_descriptors ( tocalls.size() );
  int ncolumns = 0;
  for ( unsigned int i = 0; i < tocalls.size(); i++ )
    {
      Mat descriptor;
      ( * tocalls[i] ) ( image, descriptor );
      ncolumns += descriptor.cols;
      all_descriptors[i] = descriptor;
    }

  Mat descriptors ( 1, ncolumns, CV_32FC1 );
  int copycolumn = 0;
  for ( unsigned int i = 0; i < all_descriptors.size(); i++ )
    {
      all_descriptors[i].copyTo ( Mat ( descriptors, Rect ( copycolumn, 0, all_descriptors[i].cols, all_descriptors[i].rows ) ) );
      copycolumn += all_descriptors[i].cols;
    }
  resultst.push_back ( svm.predict ( descriptors, true ) );
  resultsf.push_back ( svm.predict ( descriptors, false ) );
}

void get_freak_descriptor ( Mat image, Mat& descriptor )
{
  vector< KeyPoint > keypoints;
  freak_detector.detect ( image, keypoints );

  freak_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );
}

void is_person_present_freak ( Mat image, vector< float >& resultst, vector< float >& resultsf )
{
  Mat descriptor;
  get_freak_descriptor ( image, descriptor );

  resultst.push_back ( svm.predict ( descriptor, true ) );
  resultsf.push_back ( svm.predict ( descriptor, false ) );
}

long long getmsofday ( void )
{
  struct timeval tv;
  gettimeofday ( &tv, 0 );
  return ( long long ) tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void test_features ( string folder, vector< float >& resultst, vector< float >& resultsf )
{
  vector< string > images_filenames;
  FileUtils::read_directory ( folder, images_filenames );
  int nimages = images_filenames.size ( );
  void ( * tocall ) ( Mat, vector< float >&, vector< float >& );

  if ( feature_type.compare ( "brief" ) == 0 )
    {
      tocall = is_person_present_brief;
    }
  else if ( feature_type.compare ( "brisk" ) == 0 )
    {
      tocall = is_person_present_brisk;
    }
  else if ( feature_type.compare ( "freak" ) == 0 )
    {
      tocall = is_person_present_freak;
    }
  else if ( feature_type.compare ( "hog" ) == 0 )
    {
      tocall = is_person_present_hog;
    }
  else if ( feature_type.compare ( "sift" ) == 0 )
    {
      tocall = is_person_present_sift;
    }
  else if ( feature_type.compare ( "surf" ) == 0 )
    {
      tocall = is_person_present_surf;
    }
  else if ( feature_type.compare ( "latent" ) == 0 )
    {
      tocall = is_person_present_latentsvm;
    }
  else if ( feature_type.compare ( "cascade" ) == 0 )
    {
      tocall = is_person_present_cascade;
    }
  else if ( feature_type.compare ( "multiple" ) == 0 )
    {
      cout << "testing with multiple features" << endl;
      tocall = is_person_present_multiple;
    }
  else
    {
      return;
    }

  long long start = getmsofday();
  for ( int i = 0; i < nimages; i++ )
    {
      string stri = StringUtils::inttostr ( i, 5 );
      cout << "[" << stri << "] testing image: " << images_filenames[i];

      // reading the image in grey scale format
      Mat image = imread ( images_filenames[i] );

      ( * tocall ) ( image, resultst, resultsf );
      cout << " T: "  << resultst[i] << " F: " << resultsf[i] << endl;
    }
  long long end = getmsofday();
  printf ( "testing ml took %lld ms\n", end - start );
}

int main ( int argc, char** argv )
{
  feature_type = argv[1];

  if ( feature_type.compare ( "sift" ) == 0 )
    {
      ml_type = argv[5];
      if ( ml_type.compare ( "svm" ) == 0 )
        {
          svm.load ( argv[2] );
        }
      else if ( ml_type.compare ( "knn" ) == 0 )
        {
          knn.load ( argv[2] );
        }
      else if ( ml_type.compare ( "ann_mlp" ) == 0 )
        {
          ann_mlp.load ( argv[2] );
        }
      else if ( ml_type.compare ( "em" ) == 0 )
        {
          // FIXME: em has no load
//           em.load ( argv[2] );
        }
      else if ( ml_type.compare ( "er_trees" ) == 0 )
        {
          er_trees.load ( argv[2] );
        }
      else if ( ml_type.compare ( "r_trees" ) == 0 )
        {
          r_trees.load ( argv[2] );
        }
      else if ( ml_type.compare ( "gb_trees" ) == 0 )
        {
          gb_trees.load ( argv[2] );
        }
      else if ( ml_type.compare ( "boost" ) == 0 )
        {
          boost.load ( argv[2] );
        }
      else if ( ml_type.compare ( "d_trees" ) == 0 )
        {
          d_trees.load ( argv[2] );
        }
      else if ( ml_type.compare ( "nbc" ) == 0 )
        {
          nbc.load ( argv[2] );
        }
    }
  else if ( feature_type.compare ( "brief" ) == 0 || feature_type.compare ( "brisk" ) == 0 || feature_type.compare ( "freak" ) == 0 ||
            feature_type.compare ( "hog" ) == 0 || feature_type.compare ( "surf" ) == 0 )
    {
      svm.load ( argv[2] );
    }
  else if ( feature_type.compare ( "latent" ) == 0 )
    {
      load_latent_svm_detector ( argv[2] );
    }
  else if ( feature_type.compare ( "cascade" ) == 0 )
    {
      load_cascade_classifier ( argv[2] );
    }
  else if ( feature_type.compare ( "multiple" ) == 0 )
    {
      svm.load ( argv[2] );
      for ( int i = 6; i < argc; i++)
        {
          string multiple = argv[i];
          if ( multiple.compare ( "sift" ) == 0 )
            {
              tocalls.push_back ( get_sift_descriptor );
            }
          else if ( multiple.compare ( "brief" ) == 0 )
            {
              tocalls.push_back ( get_brief_descriptor );
            }
          else if ( multiple.compare ( "brisk" ) == 0 )
            {
              tocalls.push_back ( get_brisk_descriptor );
            }
          else if ( multiple.compare ( "freak" ) == 0 )
            {
              tocalls.push_back ( get_freak_descriptor );
            }
        }
      cout << "multiple feature testing " << tocalls.size() << endl;
    }
  else
    {
      cerr << feature_type << " is not supported" << endl;
    }

  vector< float > resultst;
  vector< float > resultsf;
  test_features ( argv[3], resultst, resultsf );

  string otfilename = argv[4];
  FileStorage fsr ( otfilename, FileStorage::WRITE );
  fsr << "new_results" << "[";
  for ( unsigned int i = 0; i < resultst.size(); i++ )
    {
      fsr << "{:" << "t" << resultst[i] << "f" << resultsf[i] << "}";
    }
  fsr.release();

  return 0;
}