#!/bin/bash
libpath=$(find $(pwd) -type f -name "libshim.so" | sed 's_/_\\/_g')
webpath=$(readlink --canonicalize web | sed 's_/_\\/_g')
ngxpath=$1

# modify nginx
sed -i 's/-Werror//' ${ngxpath}/objs/Makefile
sed -i "s/-Wl,-E/-Wl,-E ${libpath}/" ${ngxpath}/objs/Makefile
sed -i "19s/root.*;/root ${webpath};/" nginx.conf
