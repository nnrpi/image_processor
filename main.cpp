#include <iostream>
#include <vector>

#include "BMP.h"
#include "Parser.h"

inline void PrintException(std::invalid_argument& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}

inline void PrintHelp() {
    std::cout
        << "This program can apply some filters to your photo\n"
           "Your photo should be in BMP 24-bit format\n"
           "Argument format:\n"
           "\t1) path to binary file\n"
           "\t2) path to input file (your photo)\n"
           "\t3) path to output file\n"
           "\t4) some of filers you want to apply in format <-filter_name> <list of parameters if they need>\n"
           "Filters:\n"
           "\t1) crop: crop your photo from the left top angle, parameters are new height and width in pixels "
           "(integers)\n"
           "\t2) gs: convert to gray shades, no parameters need\n"
           "\t3) neg: convert to negative, no parameters need\n"
           "\t4) sharp: increase sharpness, no parameters need\n"
           "\t5) edge: highlight edges of objects in the photo, parameter is an integer number from 0 to 255\n"
           "\t6) blur: blur your photo, parameter is an integer number - the more this number the more blur applies\n"
           "\t7) acos: somehow convert the colours of your photo";
}

void ApplyFilters(BMP& image, Parser& parser) {
    for (auto& filter : parser.using_filters) {
        filter->ApplyFilter(image);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        PrintHelp();
        return 0;
    }
    try {
        Parser parser = Parser(argc, argv);
        parser.ParseArgs();
        BMP image;
        image.Read(parser.input_file);
        ApplyFilters(image, parser);
        image.Write(parser.output_file);
    } catch (std::invalid_argument& e) {
        PrintException(e);
        return -1;
    }
    return 0;
}