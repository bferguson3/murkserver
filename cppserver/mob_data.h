// mob_data.h
    std::string desc;

    int     STR;
    int     DEX;
    int     WIS;

    int     exp;
    int     dropped_gold; // always 

//std::map<int, Item*>     drop_table; // points to value in Item Database
std::map<int, Command*>  ai;         // points to value in Cmd Database 

// e.g. 
// 20  potion 
// 5   phoenix 

