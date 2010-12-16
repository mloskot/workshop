//#define BOOST_GIL_EXTERNAL_LIB ( BOOST_LIB_LINK_LOADTIME_OR_STATIC, BOOST_LIB_LOADING_STATIC, BOOST_LIB_INIT_ASSUME )
#define BOOST_EXCEPTION_DISABLE

#include "boost/gil/extension/io2/libtiff_image.hpp"

void convert_int_to_hex( unsigned int integer, wchar_t * p_char_buffer );

int __cdecl main( int /*argc*/, char * /*argv*/[] )
{
    using namespace boost;
    using namespace boost::gil;

    libtiff_image::guard const tiff_guard;

    typedef image<bgr8_pixel_t, false> tile_holder_t;

    typedef libtiff_image::reader_for<wchar_t const *>::type reader_t;
    reader_t reader( L"TrueMarble.250m.21600x21600.E2.tif" );

    /*
    std::size_t const output_tile_size( 512 );
    tile_holder_t output_tile_holder( output_tile_size, output_tile_size );
    typedef wic_image::writer_for<wchar_t const *>::type writer_t;

    wchar_t output_file_name[] = L"__out_tile__00000000.png";
    wchar_t * const p_back_of_file_name_number( boost::end( output_file_name ) - ( boost::size( L".png" ) + 1 ) );

    unsigned int                 output_tile_counter( 0                   );
    point2<std::ptrdiff_t> const input_dimensions   ( reader.dimensions() );
    for ( unsigned int x( 0 ); x < input_dimensions.x; x += output_tile_size )
    {
        for ( unsigned int y( 0 ); y < input_dimensions.y; y += output_tile_size )
        {
            reader.copy_to
            (
                reader_t::offset_view
                (
                    view( output_tile_holder ),
                    reader_t::offset_t( x, y )
                ),
                assert_dimensions_match(),
                assert_formats_match   ()
            );

            convert_int_to_hex( output_tile_counter++, p_back_of_file_name_number );
            writer_t( output_file_name, view( output_tile_holder ), png ).write_default();
        }
    }
    */
    return 0;
}

void convert_int_to_hex( unsigned int const integer, wchar_t * p_char_buffer )
{
    static wchar_t const hexDigits[] = L"0123456789ABCDEF";
    unsigned char const *       p_current_byte( reinterpret_cast<unsigned char const *>( &integer ) );
    unsigned char const * const p_end         ( p_current_byte + sizeof( integer )                  );
    while ( p_current_byte != p_end )
    {
        *p_char_buffer-- = hexDigits[ ( ( *p_current_byte ) & 0x0F )      ];
        *p_char_buffer-- = hexDigits[ ( ( *p_current_byte ) & 0xF0 ) >> 4 ];
        ++p_current_byte;
    }
}
