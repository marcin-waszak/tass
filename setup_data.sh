#!/bin/bash

mkdir -p data
mkdir -p result_data
mkdir -p maps
mkdir -p offline_data
curl -L --data "@postdata.dat" "https://www.transtats.bts.gov/DownLoad_Table.asp?Table_ID=293&Has_Group=3&Is_Zipped=0" > data/flights.zip

cd data

wget "https://raw.githubusercontent.com/jpatokal/openflights/master/data/airports.dat"
wget "http://download.geonames.org/export/dump/cities500.zip"

unzip "*.zip"
rm -rf *.zip

mv cities500.txt cities.dat
mv *T100_SEGMENT_ALL_CARRIER.csv flights.dat

cd -

