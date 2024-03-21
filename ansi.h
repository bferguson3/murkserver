//! @file ansi.h 
//! @brief ANSI shortcuts. Only used in certain situations.


#define _Esc "\x1b["
#define _Col(n) "[38;5;#nm"

/*
#define _FG_Black _Esc "[38;5;0m"
#define _FG_Red _Esc "[38;5;1m"
#define _FG_Green _Esc "[38;5;2m"
#define _FG_Yellow _Esc "[38;5;3m"
#define _FG_Blue _Esc "[38;5;4m"
#define _FG_Purple _Esc "[38;5;5m"
#define _FG_Cyan _Esc "[38;5;6m"
#define _FG_Grey _Esc "[38;5;7m"
#define _FG_BBlack _Esc "[38;5;8m"
#define _FG_BRed _Esc "[38;5;9m"
#define _FG_BGreen _Esc "[38;5;10m"
#define _FG_BYellow _Esc "[38;5;11m"
#define _FG_BBlue _Esc "[38;5;12m"
#define _FG_BPurple _Esc "[38;5;13m"
#define _FG_BCyan _Esc "[38;5;14m"
#define _FG_White _Esc "[38;5;15m"
*/
#define _FG_Black _Esc "30;m"
#define _FG_Red _Esc "31;m"
#define _FG_Green _Esc "32;m"
#define _FG_Yellow _Esc "33;m"
#define _FG_Blue _Esc "34;m"
#define _FG_Magenta _Esc "35;m"
#define _FG_Cyan _Esc "36;m"
#define _FG_White _Esc "37;m"

#define _BG_Black _Esc "40;m"
#define _BG_Red _Esc "41;m"
#define _BG_Green _Esc "42;m"
#define _BG_Yellow _Esc "43;m"
#define _BG_Blue _Esc "44;m"
#define _BG_Magenta _Esc "45;m"
#define _BG_Cyan _Esc "46;m"
#define _BG_White _Esc "47;m"

#define _SetPos(x, y) _Esc #y ";" #x "H"
#define _Rst _Esc "0m"
#define _Cls _Esc "2J"
#define _Br _Esc "E"

// SGR extended:
#define _FG_Grey _Esc "1;30;m"
#define _FG_BRed _Esc "1;31;m"
#define _FG_BGreen _Esc "1;32;m"
#define _FG_BYellow _Esc "1;33;m"
#define _FG_BBlue _Esc "1;34;m"
#define _FG_BMagenta _Esc "1;35;m"
#define _FG_BCyan _Esc "1;36;m"
#define _FG_BWhite _Esc "1;37;m"

#define _ResetCol _Esc "39;49m"
