# ansi to json
import sys,os

f = open(sys.argv[1], 'rb')
ans = f.read()
f.close()

print("\"",end='')

for b in ans:
    if((b < 32) or (b > 126)):
        print("\\\\u00" + '{:02x}'.format(b), end='')
    else:
        print(chr(b), end='')

print("\"")
