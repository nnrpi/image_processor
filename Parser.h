#ifndef CPP_PILOT_HSE_PARSER_H
#define CPP_PILOT_HSE_PARSER_H

#include <vector>
#include "Filter.h"

class Parser {
public:
    std::vector<std::unique_ptr<Filter>> using_filters;
    std::string input_file;
    std::string output_file;
    int argc = 0;
    char** argv;

    Parser(int argc, char** argv);

    bool IsNumber(const char* arg);
    void ParseCrop(size_t ind);
    void ParseGs();
    void ParseNeg();
    void ParseSharp();
    void ParseEdge(size_t ind);
    void ParseBlur(size_t ind);
    void ParseAcos();
    size_t ParseFilter(size_t ind);
    void ParseArgs();
};

#endif  // CPP_PILOT_HSE_PARSER_H
