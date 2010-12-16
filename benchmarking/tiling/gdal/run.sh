#!/bin/sh
if test -d output; then
	rm -rf output
fi
mkdir output
cd output
echo "200x200"
time ../gdal_image_tiles_test_200
rm -f *png
echo "256x256"
time ../gdal_image_tiles_test_256
rm -f *png
echo "512x512"
time ../gdal_image_tiles_test_512
echo "Done!"
cd ..
rm -rf output
