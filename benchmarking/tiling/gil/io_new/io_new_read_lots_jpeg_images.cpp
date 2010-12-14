// io2_test_vc10.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define TEST_IO_NEW

#define BOOST_FILESYSTEM_VERSION 2

#define _CRT_SECURE_NO_WARNINGS 1
#define _SCL_SECURE_NO_WARNINGS 1

#ifdef NDEBUG
#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0
#endif

#include <stdio.h>
#include <tchar.h>

#include <string>

#include <iostream>

#include <boost/timer.hpp>
#include <boost/filesystem/convenience.hpp>

#ifdef TEST_IO_NEW

#include <boost/gil/extension/io_new/jpeg_read.hpp>

using namespace std;
using namespace boost;
using namespace gil;

namespace fs = boost::filesystem;

int _tmain(int argc, _TCHAR* argv[])
{
    string in( "C:/chhenning/lots_of_jpegs/" );
    //string in( "C:/chhenn/ing/few_jpegs/" );
    fs::path in_path = fs::system_complete( fs::path( in, fs::native ) );

    try
    {

        unsigned int counter = 0;
        timer t;

        if ( fs::is_directory( in_path ) )
        {
            fs::directory_iterator end_iter;
            for( fs::directory_iterator dir_itr( in_path )
                ; dir_itr != end_iter
                ; ++dir_itr
                )
            {
                if ( fs::is_regular( dir_itr->status() ) 
                && ( fs::extension( dir_itr->path() ) == ".jpg" ))
                {
                    rgb8_image_t img;
                    string filename = in + string( dir_itr->path().leaf() );
                    read_image( filename, img, jpeg_tag() );

                    ++counter;
                }
            }
        }

        cout << "# files : " << counter     << endl;
        cout << "time    : " << t.elapsed() << " sec" << endl;
    }
    catch( std::exception& e )
    {
        cout << e.what() << endl;

        exit( 1 );
    }
}

#else

#include <io2\libjpeg_image.hpp>

using namespace std;
using namespace boost;
using namespace gil;

namespace fs = boost::filesystem;

int _tmain(int argc, _TCHAR* argv[])
{
    string in( "C:/chhenning/lots_of_jpegs/" );
    //string in( "C:/chhenn/ing/few_jpegs/" );
    fs::path in_path = fs::system_complete( fs::path( in, fs::native ) );

    try
    {

        unsigned int counter = 0;
        timer t;

        if ( fs::is_directory( in_path ) )
        {
            fs::directory_iterator end_iter;
            for( fs::directory_iterator dir_itr( in_path )
                ; dir_itr != end_iter
                ; ++dir_itr
                )
            {
                if ( fs::is_regular( dir_itr->status() ) 
                && ( fs::extension( dir_itr->path() ) == ".jpg" ))
                {
                    rgb8_image_t jpeg_test_image;
                    string filename = in + string( dir_itr->path().leaf() );
                    libjpeg_image::read( filename.c_str(), jpeg_test_image );

                    ++counter;
                }
            }
        }

        cout << "# files : " << counter     << endl;
        cout << "time    : " << t.elapsed() << " sec" << endl;
    }
    catch( std::exception& e )
    {
        cout << e.what() << endl;

        exit( 1 );
    }
}
#endif