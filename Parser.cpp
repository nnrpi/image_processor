#include "Parser.h"
#include "Filter.h"
#include <iostream>
#include <memory>

const std::vector<std::string> FILTERS = {"-crop", "-gs", "-neg", "-sharp", "-edge", "-blur", "-acos"};

bool Parser::IsNumber(const char *arg) {
    for (const char *ch = arg; *ch != '\0'; ++ch) {
        if ('0' > *ch || *ch > '9') {
            return false;
        }
    }
    return true;
}

void Parser::ParseCrop(size_t ind) {
    if (ind >= argc - 2) {
        throw std::invalid_argument("Not enough arguments for Crop filter");
    }
    if (!IsNumber(argv[ind + 1]) || !IsNumber(argv[ind + 2])) {
        throw std::invalid_argument("Arguments for Crop filter should be integers");
    }
    std::string width = argv[ind + 1];
    std::string height = argv[ind + 2];
    using_filters.push_back(std::make_unique<Crop>(std::vector{std::stoi(argv[ind + 1]), std::stoi(argv[ind + 2])}));
}

void Parser::ParseGs() {
    using_filters.push_back(std::make_unique<Gs>());
}

void Parser::ParseNeg() {
    using_filters.push_back(std::make_unique<Neg>());
}

void Parser::ParseSharp() {
    using_filters.push_back(std::make_unique<Sharp>());
}

void Parser::ParseEdge(size_t ind) {
    if (ind >= argc - 1) {
        throw std::invalid_argument("Not enough arguments for Edge filter");
    }
    if (!IsNumber(argv[ind + 1])) {
        throw std::invalid_argument("Argument for Edge filter should be integer");
    }
    using_filters.push_back(std::make_unique<Edge>(std::vector{std::stoi(argv[ind + 1])}));
}

void Parser::ParseBlur(size_t ind) {
    if (ind >= argc - 1) {
        throw std::invalid_argument("Not enough arguments for Blur filter");
    }
    if (!IsNumber(argv[ind + 1])) {
        throw std::invalid_argument("Argument for Blur filter should be integer");
    }
    using_filters.push_back(std::make_unique<Blur>(std::vector{std::stoi(argv[ind + 1])}));
}

void Parser::ParseAcos() {
    using_filters.push_back(std::make_unique<Acos>());
}

size_t Parser::ParseFilter(size_t ind) {
    std::string filter_name = argv[ind];
    if (std::find(FILTERS.begin(), FILTERS.end(), filter_name) == FILTERS.end()) {
        throw std::invalid_argument("No such filter " + filter_name);
    }
    if (filter_name == "-crop") {
        ParseCrop(ind);
        return 3;
    }
    if (filter_name == "-gs") {
        ParseGs();
        return 1;
    }
    if (filter_name == "-neg") {
        ParseNeg();
        return 1;
    }
    if (filter_name == "-sharp") {
        ParseSharp();
        return 1;
    }
    if (filter_name == "-edge") {
        ParseEdge(ind);
        return 2;
    }
    if (filter_name == "-acos") {
        ParseAcos();
        return 1;
    }
    ParseBlur(ind);
    return 2;
}

void Parser::ParseArgs() {
    if (argc >= 2) {
        input_file = argv[1];
    } else {
        throw std::invalid_argument("No path to input file");
    }
    if (argc >= 3) {
        output_file = argv[2];
    } else {
        throw std::invalid_argument("No path to output file");
    }
    if (argc >= 4) {
        size_t ind = 3;
        size_t delta;
        while (ind < argc) {
            delta = ParseFilter(ind);
            ind += delta;
        }
    }
}

Parser::Parser(int argc, char** argv) {
    this->argc = argc;
    this->argv = argv;
}
