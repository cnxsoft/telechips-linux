#! /bin/bash

if [ ! -d ./output ] ; then
	mkdir ./output
fi

cp -vf i-core/itv-core.ko                        output/
cp -vf i-platform/i-telechips/itv-tccxxxx.ko     output/
cp -vf i-demux/itv-ts-module.ko                  output/
cp -vf i-frontend/i-demod/itv-s5h1411-module.ko  output/
cp -vf i-frontend/i-tuner/itv-xc5000-module.ko   output/
cp -vf i-frontend/i-tuner/itv-fqd1136-module.ko  output/
cp -vf i-adapter/i-direct/itv-direct.ko          output/
