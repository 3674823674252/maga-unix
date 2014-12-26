#!/bin/bash

args=2
ebadargs=85
ebaddet=86
ebaddir=87
eemtra=88

apikey='trnsl.1.1.20141225T224635Z.7e7e6ca735fb92eb.ccdf830154aa5db64fbaf77e6cdfd422c731550f'

if [ $# -ne $args ]
then
  echo "`basename $0` [word] [target-lang|source-target pair]"
  exit $ebadargs
fi

word=$1

if ! echo $2 | fgrep "-" >/dev/null
then
  apidet="https://translate.yandex.net/api/v1.5/tr.json/detect?key=$apikey&text=$word"
  detres=`curl -s $apidet`
  detres=`echo $detres | awk -F'"' '{print $6}'`
  if [ -z "$detres" ]
  then
    echo "api could not detect language"
    exit $ebaddet
  else
    dir="$detres-$2"
  fi
else
  dir=$2
fi

if [ -z "$dir" ]
then
  echo "could not construct direction param"
  exit $ebaddir
fi

apitra="https://translate.yandex.net/api/v1.5/tr.json/translate?key=$apikey&lang=$dir&text=$word"
detres=`curl -s $apitra`

if ! echo $detres | fgrep "200" > /dev/null # response is not 200, error happened
then
  echo "could not translate provided word to provided language"
  exit $ebadtra
fi

tra=`echo $detres | awk -F'[' '{print $2}' | awk -F']' '{print $1}' | awk -F'"' '{print $2}'`

if [ -z "$tra" ]
then
  echo "translation is empty"
  exit $eemtra
fi

echo $tra

