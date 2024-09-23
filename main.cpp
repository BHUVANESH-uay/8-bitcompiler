#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct Instruction {
     string opcode;
     string arg1;
     string arg2;
     string arg3;
};

 unordered_map< string, int> variables;
 vector<Instruction> instructions;
int regCount = 0;

 string getRegister() {
    return "R" +  to_string(regCount++);
}

void emit(const  string &opcode, const  string &arg1, const  string &arg2 = "", const  string &arg3 = "") {
    instructions.push_back({opcode, arg1, arg2, arg3});
}

void compileAssignment(const  string &var, int value) {
     string reg = getRegister();
    emit("MOV", reg,  to_string(value));
    variables[var] = regCount - 1;
}

void compileExpression(const  string &var, const  string &lhs, const  string &op, const  string &rhs) {
     string regLhs = "R" +  to_string(variables[lhs]);
     string regRhs = "R" +  to_string(variables[rhs]);
     string resultReg = getRegister();

    if (op == "+") {
        emit("ADD", resultReg, regLhs, regRhs);
    } else if (op == "-") {
        emit("SUB", resultReg, regLhs, regRhs);
    } else if (op == "*") {
        emit("MUL", resultReg, regLhs, regRhs);
    } else if (op == "&") {
        emit("AND", resultReg, regLhs, regRhs);
    } else if (op == "|") {
        emit("OR", resultReg, regLhs, regRhs);
    } else if (op == "^") {
        emit("XOR", resultReg, regLhs, regRhs);
    }

    variables[var] = regCount - 1;
}

void compile(const  vector< string> &source) {
    for (const auto &line : source) {
        if (line.find("let") !=  string::npos) {
            size_t equalPos = line.find("=");
             string var = line.substr(4, equalPos - 5);
            int value =  stoi(line.substr(equalPos + 2));
            compileAssignment(var, value);
        } else if (line.find("=") !=  string::npos) {
            size_t equalPos = line.find("=");
             string var = line.substr(0, equalPos - 1);
            size_t opPos = line.find_first_of("+-*&|^");
             string lhs = line.substr(equalPos + 2, opPos - equalPos - 3);
             string op = line.substr(opPos, 1);
             string rhs = line.substr(opPos + 2);
            compileExpression(var, lhs, op, rhs);
        } else if (line.find("PUSH") !=  string::npos) {
             string var = line.substr(5);
            emit("PUSH", "R" +  to_string(variables[var]));
        } else if (line.find("POP") !=  string::npos) {
             string var = line.substr(4);
            emit("POP", "R" +  to_string(variables[var]));
        }
    }
}

void printAssembly( ofstream &outputFile) {
    for (const auto &inst : instructions) {
        outputFile << inst.opcode << " " << inst.arg1;
        if (!inst.arg2.empty()) outputFile << ", " << inst.arg2;
        if (!inst.arg3.empty()) outputFile << ", " << inst.arg3;
        outputFile <<  endl;
    }
}

 vector< string> readSource(const  string &filename) {
     vector< string> source;
     ifstream file(filename);
     string line;
    while ( getline(file, line)) {
        source.push_back(line);
    }
    return source;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
         cerr << "Usage: " << argv[0] << " <source_file>" <<  endl;
        return 1;
    }
     string sourceFile = argv[1];
     vector< string> source = readSource(sourceFile);
    compile(source);

     ofstream outputFile("output.asm");
    printAssembly(outputFile);
    outputFile.close();

    return 0;
}