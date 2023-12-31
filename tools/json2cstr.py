#!/usr/bin/python3
# json2cstr.py
import os,sys

f = open(sys.argv[1], "r")
inby = f.read()
f.close()

outby=''

i = 0
while i < len(inby):
    if(ord(inby[i]) == 0x22): # quote 
        outby += "\\\""
    elif(ord(inby[i])==0x0a): # newline 
        outby += "\\\n"
    else: 
        outby += inby[i]
    i+=1

print("const char* " + os.path.splitext(os.path.basename(sys.argv[1]))[0] + 
    "_json = \"" + outby + "\";\n")
