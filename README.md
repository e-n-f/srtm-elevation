srtm-elevation
==============

Turns SRTM hgt.zip files into a PNG with 20 pixels per degree.

    cc -g -Wall -O3 world.c -lm -lpng16
    find dds.cr.usgs.gov/srtm/version2_1/SRTM3 -name '*.zip' -print | ./a.out > srtm.png
