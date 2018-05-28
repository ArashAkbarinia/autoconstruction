/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "GtUtils.h"

#include "../../../src/utils/FileUtils.h"
#include "../../../src/utils/StringUtils.h"

void GtUtils::create_combine_gts_vector ( const vector< string > filenames, vector< vector< Point > >& gts )
{
  // TODO: use pre allocated memory rather than push_backs
  CvUtils cv_utils;
  for ( unsigned int i = 0; i < filenames.size(); i++ )
    {
      vector< Rect > cur_gts;
      FileUtils::read_rects_from_file ( filenames[i], cur_gts );

      for ( unsigned int j = 0; j < cur_gts.size(); j++ )
        {
          if ( gts.size() > j )
            {
              gts[j].push_back ( cv_utils.get_rect_centre ( cur_gts[j] ) );
            }
          else
            {
              vector< Point > points;
              points.push_back ( cv_utils.get_rect_centre ( cur_gts[j] ) );
              gts.push_back ( points );
            }
        }
    }
}

void GtUtils::create_combine_gts_vector ( const vector< string > filenames, vector< vector< Rect > >& gts )
{
  // TODO: use pre allocated memory rather than push_backs
  CvUtils cv_utils;
  for ( unsigned int i = 0; i < filenames.size(); i++ )
    {
      vector< Rect > cur_gts;
      FileUtils::read_rects_from_file ( filenames[i], cur_gts );

      for ( unsigned int j = 0; j < cur_gts.size(); j++ )
        {
          if ( gts.size() > j )
            {
              gts[j].push_back ( cur_gts[j] );
            }
          else
            {
              vector< Rect > rects;
              rects.push_back ( cur_gts[j] );
              gts.push_back ( rects );
            }
        }
    }
}

void GtUtils::compare_gts_results ( const vector< vector< Point > > gts, const vector< vector< Point > > results, const string output_file )
{
  ConfusionMatrix eval;
  unsigned int nframes = gts.size ( );
  if ( nframes == 0 )
    {
      // nothing to be compares
      return;
    }
  unsigned int npeople = gts[0].size ( );

  vector< vector< Point > > gt_per_person ( npeople );
  vector< vector< Point > > results_per_person ( npeople );
  vector< vector< Point > > outliers_per_frame ( nframes );
  double outliers_ratio = 0;

  for ( unsigned int i = 0; i < nframes; i++ )
    {
      vector< Point > matching;
      vector< Point > outliers;
      ConfusionMatrix eval_temp = data_association ( gts[i], results[i], matching, outliers );
      eval.tp += eval_temp.tp;
      eval.tn += eval_temp.tn;
      eval.fp += eval_temp.fp;
      eval.fn += eval_temp.fn;
      // TODO: instead of push_back use pre allocated memory
      for ( unsigned int j = 0; j < npeople; j++ )
        {
          gt_per_person[j].push_back ( gts[i].at ( j ) );
          results_per_person[j].push_back ( matching[j] );
        }
      outliers_per_frame[i] = outliers;
      outliers_ratio += outliers.size();
    }

  for ( unsigned int j = 0; j < npeople; j++ )
    {
      string file_name = output_file;
      file_name.append ( "-person" );
      file_name.append ( StringUtils::inttostr ( j + 1, 2 ) );
      compare_gts_results ( gt_per_person[j], results_per_person[j], file_name );
    }

  outliers_ratio /= nframes;

  FileUtils::write_result_report_to_file ( output_file, outliers_per_frame, outliers_ratio, eval );
}

void GtUtils::compare_gts_results ( const vector< Point > gts, const vector< Point > results, const string filename )
{
  CvUtils cv_utils;
  // Compute distance
  vector< double > distance;
  double d;
  double sum = 0;
  for ( unsigned int i = 0; i < results.size(); i++ )
    {
      // FIXME: check if this is correct
      if ( results[i].x == 0 && results[i].y == 0 )
        {
          d = 0;
        }
      else
        {
          d = cv_utils.euclidean_distance ( gts[i], results[i] );
        }
      distance.push_back ( d );
      sum += d;
    }

  // Compute mean of distances
  double size_d = results.size();
  double mean_d = sum / size_d;

  // Compute mse
  double s_mse = 0;
  for ( int i = 0; i < size_d; i++ )
    {
      s_mse += pow ( distance[i], 2 );
    }

  double mse = s_mse / size_d;

  FileUtils::write_distances_to_file ( filename, distance, mse, mean_d );
}

ConfusionMatrix GtUtils::data_association ( const vector< Point > gts, const vector< Point > results, vector< Point >& matching, vector< Point >& outliers )
{
  ConfusionMatrix conf_mat;
  Mat mat_compare ( gts.size ( ), results.size ( ), CV_32FC1 );
  CvUtils cv_utils;

  // TODO: instead of push_back use pre allocated memory for matching
  if ( results.empty() )
    {
      for ( unsigned int i = 0; i < gts.size(); i++ )
        {
          matching.push_back ( Point ( 0, 0 ) );
          if ( gts[i].x == 0 && gts[i].y == 0 )
            {
              conf_mat.tn++;
            }
          else
            {
              conf_mat.fn++;
            }
        }
      return conf_mat;
    }

  // create the distance matrix
  for ( unsigned int i = 0; i < gts.size(); i++ )
    {
      for ( unsigned int j = 0; j < results.size(); j++ )
        {
          mat_compare.at< float > ( i, j ) = cv_utils.euclidean_distance ( gts[i], results[j] );
        }
    }

  float distance_threshold = 100.0;
  vector< bool > min_locations ( results.size ( ), false );
  for ( int k = 0; k < mat_compare.rows; k++ )
    {
      // if ground truth is empty we don't need to do much, we just put that row as distance_threshold
      if ( gts[k].x == 0 && gts[k].y == 0 )
        {
          conf_mat.tn++;
          mat_compare.row ( k ) = Scalar::all ( distance_threshold );
          matching.push_back ( Point ( 0, 0 ) );
          continue;
        }

      double minvalrow, minvalcol;
      Point minlocrow, minloccol;
      minMaxLoc ( mat_compare.row ( k ), &minvalrow, NULL, &minlocrow, NULL );
      bool row_done = false;

      while ( minvalrow < distance_threshold )
        {
          minMaxLoc ( mat_compare.col ( minlocrow.x ), &minvalcol, NULL, &minloccol, NULL );
          if ( minloccol.y == k )
            {
              matching.push_back ( results[minlocrow.x] );
              min_locations[minlocrow.x] = true;
              conf_mat.tp++;
              mat_compare.col ( minlocrow.x ) = Scalar::all ( distance_threshold );
              row_done = true;
              break;
            }
          else
            {
              mat_compare.at< float > ( k, minlocrow.x ) = distance_threshold; // so in the next look this is not selected
              minMaxLoc ( mat_compare.row ( k ), &minvalrow, NULL, &minlocrow, NULL ); // selecting the second minimum, since the first minimum belongs to another gt
            }
        }
      // else no result was close enough to this ground truth
      if ( !row_done )
        {
          matching.push_back ( Point ( 0, 0 ) );
          conf_mat.fn++;
        }
    }

  for ( unsigned int k = 0; k < min_locations.size(); k++ )
    {
      if ( !min_locations[k] )
        {
          outliers.push_back ( results[k] );
          conf_mat.fp++;
        }
    }

  return conf_mat;
}