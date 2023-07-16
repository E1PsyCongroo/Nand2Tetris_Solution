/*handles symbols*/
#include <map>
#include <string>
using std::map; using std::string;
class SymbolTable {
    public:
    SymbolTable(string filename);
    bool contains(string);
    int getAddress(string);
    private:
    map<string, int> _table;
    void addEntry(string, int);
    bool isNumeric(const string& str);
};