# index blobs/include and make a res h file for it 
import os,sys

fp = sys.argv[1]

if(fp[len(fp)-1] != '/'):
    fp += '/'

files = os.listdir(fp)

for f in files:
    print("#include \"" + fp + f + "\"")