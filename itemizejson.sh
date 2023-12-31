exec 1>>jsonres.h
echo // Do not modify this file!!
for f in blobs/include/*
do 
    echo \#include \"$f\" 
done
