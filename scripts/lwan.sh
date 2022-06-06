#!/usr/bin/env bash
libpath=$(find $(pwd) -type f -name "libshim.so" | sed 's_/_\\/_g')
webpath=$(readlink --canonicalize web | sed 's_/_\\/_g')
lwanpath=$1
# modify lwan
sed -i "26s/path.*/path = ${webpath}/" configs/lwan.conf
sed -i "37a add_library(libshim SHARED IMPORTED GLOBAL)\nset_target_properties(libshim PROPERTIES IMPORTED_LOCATION ${libpath})\nlist(APPEND ADDITIONAL_LIBRARIES libshim)" ${lwanpath}/CMakeLists.txt