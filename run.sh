#!/bin/bash

mkdir -p data
curl -L --data "@postdata.dat" "https://www.transtats.bts.gov/DownLoad_Table.asp?Table_ID=293&Has_Group=3&Is_Zipped=0" > data/flights.zip

cd data

wget "https://raw.githubusercontent.com/jpatokal/openflights/master/data/airports.dat"
wget "http://download.geonames.org/export/dump/cities500.zip"

#mv cities500.txt cities.csv
unzip "*.zip"
rm -rf *.zip

cd -
