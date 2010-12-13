#include <gdal_priv.h>
#include <cpl_conv.h>
#include <boost/range/functions.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
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

dataset_t create_tile_mem_dataset(unsigned int tile_xsize, unsigned int tile_ysize)
{
    GDALDriver* drv = GetGDALDriverManager()->GetDriverByName("MEM");
    if (0 == drv)
    {
        throw std::runtime_error("mem driver not found");
    }

    dataset_t ds(make_dataset(drv->Create("", tile_xsize, tile_ysize, 3, ::GDT_Byte, 0)));
    if(!ds)
    {
        throw std::runtime_error("failed to create mem dataset");
    }
    return ds;
}

void save_tile_as_png(dataset_t dstile, char const* const file)
{
    GDALDriver* drv = GetGDALDriverManager()->GetDriverByName("PNG");
    if (0 == drv)
    {
        throw std::runtime_error("png driver not found");
    }

    dataset_t dspng(make_dataset(drv->CreateCopy(file, dstile.get(), false, 0, 0, 0)));
    if(!dspng)
    {
        throw std::runtime_error("failed to create png dataset");
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
        dataset_t input(open_dataset(input_file_name));

        char output_file_name[] = "__out_tile__00000000.png";
        char* const p_back_of_file_name_number(
                         boost::end(output_file_name) - (boost::size(".png") + 1));

#if defined(TEST_TILE_256)
        unsigned int const output_tile_size(256);
#elif defined(TEST_TILE_512)
        unsigned int const output_tile_size(512);
#else
        unsigned int const output_tile_size(200);
#endif
        int const xsize = input->GetRasterXSize();
        int const ysize = input->GetRasterYSize();
        int const nbands = input->GetRasterCount();
        assert(3 == nbands);

        unsigned int output_tile_counter(0);
        for (int x(0); x < xsize; x += output_tile_size)
        {
            unsigned int const tile_xsize = x + output_tile_size > xsize
                ? xsize - x : output_tile_size;

            for (int y(0); y < ysize; y += output_tile_size)
            {
                unsigned int const tile_ysize = y + output_tile_size > ysize
                    ? ysize - y : output_tile_size;

                raster_data_t data(tile_xsize * tile_ysize * nbands);

                read(input, data, x, y, tile_xsize, tile_ysize, nbands);

                convert_int_to_hex(output_tile_counter++, p_back_of_file_name_number);
                dataset_t output(create_tile_mem_dataset(tile_xsize, tile_ysize));
                write(output, data, 0, 0, tile_xsize, tile_ysize, nbands);
                save_tile_as_png(output, output_file_name);

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
