// Benchmark of raster tiling using GDAL
//
// Copyright (c) 2010 Mateusz Loskot <mateusz@loskot.net>
// Copyright (c) 2010 Domagoj Saric
//
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Preprocessor definitions:
// TEST_TILE_200
// TEST_TILE_256
// TEST_TILE_512
// TEST_OUTPUT_JPG (default output is PNG)
//
#include <gdal_priv.h>
#include <cpl_conv.h>
#include <boost/range/functions.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

typedef boost::shared_ptr<GDALDataset> dataset_t;
typedef std::vector<unsigned char> raster_data_t;

dataset_t make_dataset(GDALDataset* pds)
{
    dataset_t ds(pds, ::GDALClose);
    return ds;
}
dataset_t open_dataset(char const* const file)
{
    dataset_t ds(make_dataset(static_cast<GDALDataset*>(::GDALOpen(file, ::GA_ReadOnly))));
    if(!ds)
    {
        throw std::runtime_error("failed to open dataset");
    }
    return ds;
}

GDALDriver& find_driver(char const* const name)
{
    GDALDriver* drv = GetGDALDriverManager()->GetDriverByName(name);
    if (0 == drv)
    {
        throw std::runtime_error("driver not found");
    }
    return *drv;
}

dataset_t create_tile_dataset(GDALDriver& drv, unsigned int tile_xsize, unsigned int tile_ysize)
{
    dataset_t ds(make_dataset(drv.Create("", tile_xsize, tile_ysize, 3, ::GDT_Byte, 0)));
    if(!ds)
    {
        throw std::runtime_error("failed to create output dataset");
    }
    return ds;
}

void save_tile(GDALDriver& drv, dataset_t dstile, char const* const file)
{
    dataset_t dspng(make_dataset(drv.CreateCopy(file, dstile.get(), false, 0, 0, 0)));
    if(!dspng)
    {
        throw std::runtime_error("failed to save tile");
    }
}

void read(dataset_t ds, raster_data_t& data, int x, int y, int xs, int ys, int nbands)
{
    assert(data.size() == xs * ys * nbands); 

    ::CPLErr err = ds->RasterIO(::GF_Read, x, y, xs, ys, &data[0],
                                xs, ys, ::GDT_Byte, nbands, 0, 0, 0, 0);
    if (::CE_None != err)
    {
        throw std::runtime_error("raster read failed");
    }
}

void write(dataset_t ds, raster_data_t const& data, int x, int y, int xs, int ys, int nbands)
{
    assert(data.size() == xs * ys * nbands); 

    ::CPLErr err = ds->RasterIO(::GF_Write, x, y, xs, ys,
                                const_cast<raster_data_t::value_type*>(&data[0]),
                                xs, ys, ::GDT_Byte, nbands, 0, 0, 0, 0);
    if (::CE_None != err)
    {
        throw std::runtime_error("raster write failed");
    }
}

void convert_int_to_hex(unsigned int const integer, char* p_char_buffer)
{
    static char const hexDigits[] = "0123456789ABCDEF";
    unsigned char const* p_current_byte(reinterpret_cast<unsigned char const *>(&integer));
    unsigned char const* const p_end(p_current_byte + sizeof(integer));
    while (p_current_byte != p_end)
    {
        *p_char_buffer-- = hexDigits[ ( ( *p_current_byte ) & 0x0F )      ];
        *p_char_buffer-- = hexDigits[ ( ( *p_current_byte ) & 0xF0 ) >> 4 ];
        ++p_current_byte;
    }
}

int main()
{
    using namespace std;

    try
    {
        ::GDALAllRegister();

        char input_file_name[] = "/home/mloskot/data/truemarble/TrueMarble.250m.21600x21600.E2.tif";

#ifdef TEST_OUTPUT_JPG
        char const* const output_driver_name = "JPEG";
        char output_file_name[] = "__out_tile__00000000.jpg";
        char* const p_back_of_file_name_number(
                         boost::end(output_file_name) - (boost::size(".jpg") + 1));
#else
        char const* const output_driver_name = "PNG";
        char output_file_name[] = "__out_tile__00000000.png";
        char* const p_back_of_file_name_number(
                         boost::end(output_file_name) - (boost::size(".png") + 1));
#endif

#if defined(TEST_TILE_256)
        unsigned int const output_tile_size(256);
#elif defined(TEST_TILE_512)
        unsigned int const output_tile_size(512);
#else
        unsigned int const output_tile_size(200);
#endif

        GDALDriver& memory_driver = find_driver("MEM");
        GDALDriver& output_driver = find_driver(output_driver_name);

        dataset_t input_dataset(open_dataset(input_file_name));
        int const nbands = input_dataset->GetRasterCount();
        assert(3 == nbands);
        int const xsize = input_dataset->GetRasterXSize();
        int const ysize = input_dataset->GetRasterYSize();
        unsigned int output_tile_counter(0);

        unsigned int tile_xsize = output_tile_size;
        unsigned int tile_ysize = output_tile_size;
        raster_data_t data(tile_xsize * tile_ysize * nbands);

        for (int x(0); x < xsize; x += output_tile_size)
        {
            tile_xsize = x + output_tile_size > xsize ? xsize - x : output_tile_size;

            for (int y(0); y < ysize; y += output_tile_size)
            {
                tile_ysize = y + output_tile_size > ysize ? ysize - y : output_tile_size;
                unsigned int const tile_size = tile_xsize * tile_ysize * nbands;
                if (data.size() != tile_size)
                    data.resize(tile_size);

                read(input_dataset, data, x, y, tile_xsize, tile_ysize, nbands);

                convert_int_to_hex(output_tile_counter++, p_back_of_file_name_number);
                dataset_t tile_dataset(create_tile_dataset(memory_driver, tile_xsize, tile_ysize));

                write(tile_dataset, data, 0, 0, tile_xsize, tile_ysize, nbands);
                save_tile(output_driver, tile_dataset, output_file_name);

#ifndef NDEBUG
                if (tile_xsize != output_tile_size || tile_ysize != output_tile_size)
                    clog << output_file_name << " " << tile_xsize << "\tx\t" << tile_ysize << endl;
#endif
            }
        }
    }
    catch (exception const& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}
