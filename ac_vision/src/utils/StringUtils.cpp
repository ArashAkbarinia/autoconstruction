#include "StringUtils.h"

#include <sstream>
#include <time.h>

string StringUtils::inttostr ( int x )
{
  stringstream ss;
  ss << x;
  return ss.str();
}

string StringUtils::inttostr ( int x, int length )
{
  string xstr = inttostr ( x );
  string result;

  int length_difference = length - xstr.length();
  for ( int i = 0; i < length_difference; i++ )
    {
      result.append ( "0" );
    }

  result.append ( xstr );
  return result;
}

string StringUtils::doubletostr ( double x )
{
  ostringstream ss;
  ss << x;
  return ss.str();
}

string StringUtils::current_log_time ( void )
{
  time_t now;
  time ( &now );

  char buffer [80];
  strftime ( buffer, 20, "%Y%m%d%H%M%S", localtime ( &now ) );
  string log_time = buffer;
  cout << log_time << endl;

  int i;
  int length = log_time.length();
  for ( i = 0; i < length; i++ )
    {
      char c = log_time[i];
      if ( c < '0' || c > '9' )
        {
          //log_time.erase(i, 1);
        }
    }

  return log_time;
}