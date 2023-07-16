/*generates binary codes*/
#include <bitset>
#include "Code.hpp"
using std::string;
namespace Code {
    string dest(string d) {
        return destMap.at(d);
    }
    string comp(string c) {
        return compMap.at(c);
    }
    string jump(string j) {
        return jumpMap.at(j);
    }
    string dTob(int d) {
        return std::bitset<15>(d).to_string();
    }
}