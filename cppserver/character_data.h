// character_data.h
int   level;
            long   exp;

             int   STR;
             int   DEX;
             int   WIS;

std::vector<Item>  inventory; // pointers to itemdb as items are static, count is held here
std::vector<Item*> equipment;  // points to a* in inventory<a,b>
