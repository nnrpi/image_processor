#ifndef OIMP_PROJECT_BMP_H
#define OIMP_PROJECT_BMP_H

#pragma once

#include <vector>
#include <fstream>
#include <array>

class Header {
public:
    int file_size = 0;
    int offset = 0;

    void Read(std::ifstream& input_file);
    void Write(std::ofstream& output_file, int size);
};

class DIB {
    int header_len_ = 40;
    int resolution_ = 24;
    int planes_number_ = 1;
    int bitmap_data_ = 16;
    int dpi_ = 2835;

public:
    int width = 0;
    int height = 0;

    void Read(std::ifstream& input_file);
    void Write(std::ofstream& output_file);
};

class Colour {
public:
    uint8_t blue = 0;
    uint8_t green = 0;
    uint8_t red = 0;

    Colour() = default;
    explicit Colour(const std::array<uint8_t, 3>& components);

    void Read(std::ifstream& input_file);
    void Write(std::ofstream& output_file);
};

class ColourRow {
public:
    std::vector<Colour> colours;
    int32_t padding = 0;

    explicit ColourRow(size_t width);

    void Read(std::ifstream& input_file);
    void Write(std::ofstream& output_file, int width);
};

class PixelArray {
public:
    std::vector<ColourRow> rows;
    size_t rows_number = 0;
    size_t rows_size = 0;
    void Make(int height, int width);

    void Read(std::ifstream& input_file);
    void Write(std::ofstream& output_file);
};

class BMP {
public:
    Header header;
    DIB dib;
    PixelArray pixel_array;
    int width = 0;
    int height = 0;
    int size = 0;

    void Read(const std::string& input_file_name);
    void Write(const std::string& output_file_name);
    void SetHeight(int new_height);
    void SetWidth(int new_width);
    void RenewSize();
    Colour CountNewColour(const std::array<std::array<int, 3>, 3>& matrix, size_t i, size_t j);
    void ApplyMatrix(const std::array<std::array<int, 3>, 3>& matrix);
    Colour CountVertical(size_t sigma, size_t x0, size_t y0, double pi_constant, double exp_constant);
    Colour CountHorizontal(size_t sigma, size_t x0, size_t y0, double pi_constant, double exp_constant);
};

#endif //OIMP_PROJECT_BMP_H