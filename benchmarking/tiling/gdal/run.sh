#!/bin/sh
if test -d output; then
	rm -rf output
fi
mkdir output
cd output
echo "200x200"
echo 3 | sudo tee -a /proc/sys/vm/drop_caches
echo 0 | sudo tee -a /proc/sys/vm/drop_caches
time ../gdal_image_tiles_test_200
rm -f *png
echo "256x256"
echo 3 | sudo tee -a /proc/sys/vm/drop_caches
echo 0 | sudo tee -a /proc/sys/vm/drop_caches
time ../gdal_image_tiles_test_256
rm -f *png
echo "512x512"
echo 3 | sudo tee -a /proc/sys/vm/drop_caches
echo 0 | sudo tee -a /proc/sys/vm/drop_caches
time ../gdal_image_tiles_test_512
echo "Done!"
cd ..
rm -rf output
echo 0 | sudo tee -a /proc/sys/vm/drop_caches