Requires libs enet:<br>
https://github.com/lsalzman/enet <br>
and sqlite3:<br>
https://www.sqlite.org/src/zip/sqlite.zip?r=release<br>
<br>
## Doxygen
https://bferguson3.github.io/murkserver/

## Overview
MURK Flow 

< Server > 
- Uses local data files - css, json
- ENet init - 3 channels, game, comms, and broadcast. All packets are json format. 
- SQLite3 init - uses local users.db file 
- Main loop
- If request new: 
    - Send creation flow, update users.db
- If resume:
    - Loads from users.db into memory 
- User-based delay: 
    - Each user in memory has a 1000ms timer and is only processed once that timer is reached 
- Database update:
    - On user logout 
    - Occasionally otherwise, on level up?
- Information channels:
    - Game: All game related data
    - Communication: local, zone, global, whisper, party, guild 
    - Broadcast: Server notifications, etc.

< Client > 
- ENet init 
- Terminal setup 
- Create and send login packet 
    - Receive: welcome/main menu packet
        - Generally new or resume etc.
- Main loop
    - Process input and ENet events simultaneously 
        - Input is a verb - object - subject parser. e.g. GIVE APPLE PlayerB. Only keywords are used, keywords are stored in an array called InputContext and change depending on location etc.
    - Based on server delay, user usually has to wait < 1000ms for a return on their input 
    - Local character data is only updated via server packets, and is updated piecemeal after the initial log-in, which obtains the full structure from memory. 
    - All net events can occur while input is still taking place. 

Classes:
### Packet
  - Contains user ID 
  - JSON blob with packet type and accompanying data
    
### Mob   
  - Contains all custom stats as defined, as well as behavior, drop tables, etc. Generally empty 	otherwise.
    
### Screen 
  - Exits, description (short and long), and running list of mobs, items, and players. Screens will 	re-populate themselves every so many ticks.
    
### User 
  - Holds all character data
    
### Command
  - General action structure - Name, description, pointer to callback. Pointers to these commands 	make up enemy AI tables.
    
### Item
  - Name, desc, stackable, callback, stack ct max, flags, drop/destroy
    

###  blobs/
json blob packets
### tools/
misc 
### ansi.h
stdout ansi codes 
### res/base64.c/h
base64 encoding/decoding
### murk.h
cross platform header information 
### res/getpasswd.c 
filter input from stdin
### res/guid.c/h
generating unique IDs
### res/json.h
https://github.com/sheredom/json.h
### jsonres.h
generated from itemizejson.sh
### res/sha3.c/h
sha3-512 hashing source 
