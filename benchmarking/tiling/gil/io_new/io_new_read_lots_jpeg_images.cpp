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

unsigned int counter = 0;


#ifdef TEST_IO_NEW

#include <boost/gil/extension/io_new/jpeg_read.hpp>

using namespace std;
using namespace boost;
using namespace gil;

namespace fs = boost::filesystem;

rgb8_image_t img = rgb8_image_t( 136, 98 );


void read_image( const fs::path& dir_path )
{
    if ( fs::is_directory( dir_path ) )
    {
        fs::directory_iterator end_iter;
        for( fs::directory_iterator dir_itr( dir_path )
            ; dir_itr != end_iter
            ; ++dir_itr
            )
        {
            if (    fs::is_regular( dir_itr->status() ) 
               && ( fs::extension   ( dir_itr->path() ) == ".jpg" )
               )
            {
                string filename = dir_itr->path().string();
                read_image( filename, img, jpeg_tag() );

                ++counter;
            }
            else if( is_directory(dir_itr->status()) )
            {
                read_image( dir_itr->path() );
            }
        }
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    string in( "C:/chhenning/lots_of_jpegs/" );
    //string in( "C:/chhenning/few_jpegs/" );
    fs::path in_path = fs::system_complete( fs::path( in, fs::native ) );

    try
    {
        timer t;

        read_image( in_path );

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

rgb8_image_t img = rgb8_image_t( 136, 98 );

void read_image( const fs::path& dir_path )
{
    if ( fs::is_directory( dir_path ) )
    {
        fs::directory_iterator end_iter;
        for( fs::directory_iterator dir_itr( dir_path )
            ; dir_itr != end_iter
            ; ++dir_itr
            )
        {
            if (    fs::is_regular( dir_itr->status() ) 
               && ( fs::extension   ( dir_itr->path() ) == ".jpg" )
               )
            {
                string filename = dir_itr->path().string();
                libjpeg_image::read( filename.c_str(), img );

                ++counter;
            }
            else if( is_directory(dir_itr->status()) )
            {
                read_image( dir_itr->path() );
            }
        }
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    string in( "C:/chhenning/lots_of_jpegs/" );
    //string in( "C:/chhenning/few_jpegs/" );
    fs::path in_path = fs::system_complete( fs::path( in, fs::native ) );

    try
    {
        timer t;

        read_image( in_path );

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