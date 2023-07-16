/*reads and parses an instruction*/
#include <fstream>
#include <string>
using std::ifstream; using std::string;
class Parser
{
    public:
    Parser(string filename);
    ~Parser();
    enum class Type{A_INSTRUCTION, C_INSTRUCTION, L_INSTRUCTION, EMPTY};
    bool hasMoreLines();
    void advance();
    Type instructionType();
    string symbol();
    string dest();
    string comp();
    string jump();
    string curIns();
    void reset();

    private:
    ifstream source;
    string curInstruction;
    void getInstruction();
};