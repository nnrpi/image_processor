#include "BMP.h"
#include <array>
#include <exception>
#include <stdexcept>
#include <cmath>
#include <algorithm>


void Header::Read(std::ifstream &input_file) {
    char* file_type = new char[2];
    input_file.read(file_type, 2);
    if (!(file_type[0] == 'B' && file_type[1] == 'M')) {
        throw std::invalid_argument("Input file should be in BMP format");
    }
    input_file.read(reinterpret_cast<char *>(&file_size), sizeof(int));
    input_file.ignore(4);
    input_file.read(reinterpret_cast<char *>(&offset), sizeof(int));
}

void Header::Write(std::ofstream &output_file, int size) {
    output_file.write("BM", 2);
    output_file.write(reinterpret_cast<const char *>(&size), 4);
    output_file.write("\0\0\0\0", 4);
    output_file.write(reinterpret_cast<const char *>(&offset), 4);
};

void DIB::Read(std::ifstream &input_file) {
    input_file.ignore(4);
    input_file.read(reinterpret_cast<char *>(&width), sizeof(int));
    input_file.read(reinterpret_cast<char *>(&height), sizeof(int));
    input_file.ignore(28);
}

void DIB::Write(std::ofstream &output_file) {
    output_file.write(reinterpret_cast<const char *>(&header_len_), 4);
    output_file.write(reinterpret_cast<char *>(&width), sizeof(int));
    output_file.write(reinterpret_cast<char *>(&height), sizeof(int));
    output_file.write(reinterpret_cast<const char *>(&planes_number_), 2);
    output_file.write(reinterpret_cast<const char *>(&resolution_), 2);
    output_file.write("\0\0\0\0", 4);
    output_file.write(reinterpret_cast<const char *>(&bitmap_data_), 4);
    output_file.write(reinterpret_cast<const char *>(&dpi_), 4);
    output_file.write(reinterpret_cast<const char *>(&dpi_), 4);
    output_file.write("\0\0\0\0", 4);
    output_file.write("\0\0\0\0", 4);
}

void Colour::Read(std::ifstream &input_file) {
    input_file.read(reinterpret_cast<char *>(&blue), 1);
    input_file.read(reinterpret_cast<char *>(&green), 1);
    input_file.read(reinterpret_cast<char *>(&red), 1);
}

void Colour::Write(std::ofstream &output_file) {
    output_file.write(reinterpret_cast<const char *>(&blue), 1);
    output_file.write(reinterpret_cast<const char *>(&green), 1);
    output_file.write(reinterpret_cast<const char *>(&red), 1);
}

Colour::Colour(const std::array<uint8_t, 3>& components) {
    blue = components[0];
    green = components[1];
    red = components[2];
}

ColourRow::ColourRow(size_t width) {
    colours.assign(width, Colour());
    padding = (4 - (3 * static_cast<int>(width)) % 4) % 4;
}

void ColourRow::Read(std::ifstream &input_file) {
    for (auto& colour : colours) {
        colour.Read(input_file);
    }
    for (int i = 0; i < padding; ++i) {
        input_file.ignore(1);
    }
}

void ColourRow::Write(std::ofstream &output_file, int width) {
    for (auto& colour: colours) {
        colour.Write(output_file);
    }
    padding = (4 - (3 * width) % 4) % 4;
    int32_t writing_colour = 0;
    for (int i = 0; i < padding; ++i) {
        output_file.write(reinterpret_cast<const char*>(writing_colour), 1);
    }
}

void PixelArray::Make(int height, int width) {
    rows_number = static_cast<size_t>(std::abs(height));
    rows_size = static_cast<size_t>(width);
    rows.assign(rows_number, ColourRow(width));
}

void PixelArray::Read(std::ifstream &input_file) {
    for (size_t i = 0; i < rows_number; ++i) {
        rows[i].Read(input_file);
    }
}

void PixelArray::Write(std::ofstream &output_file) {
    for (ColourRow colour_row: rows) {
        colour_row.Write(output_file, static_cast<int>(rows_size));
    }
}

void BMP::RenewSize() {
    size = 54 + std::abs(height) * (width * 3 + 4 - (width * 3) % 4);
}

void BMP::Read(const std::string& input_file_name) {
    std::ifstream input_file(input_file_name, std::ios::in | std::ios::binary);
    if (input_file.is_open()) {
        header.Read(input_file);
        dib.Read(input_file);
        width = dib.width;
        height = dib.height;
        RenewSize();
        pixel_array.Make(height, width);
        pixel_array.Read(input_file);
        input_file.close();
    } else {
        throw std::invalid_argument("Not valid path to input file or not valid input file: " + input_file_name);
    }
}

void BMP::Write(const std::string& output_file_name) {
    std::ofstream output_file(output_file_name, std::ios::out | std::ios::binary);
    if (output_file.is_open()) {
        header.Write(output_file, size);
        dib.Write(output_file);
        pixel_array.Write(output_file);
        output_file.close();
    } else {
        throw std::invalid_argument("Not valid path to output file: " + output_file_name);
    }
}

void BMP::SetHeight(int new_height) {
    for (size_t i = new_height; i < height; ++i) {
        pixel_array.rows.pop_back();
    }
    pixel_array.rows_number = std::min(new_height, height);
    height = std::min(height, new_height);
    dib.height = height;
    RenewSize();
}

void BMP::SetWidth(int new_width) {
    for (auto &row: pixel_array.rows) {
        for (size_t i = new_width; i < width; ++i) {
            row.colours.pop_back();
        }
    }
    pixel_array.rows_size = std::min(width, new_width);
    width = std::min(width, new_width);
    dib.width = width;
    RenewSize();
}

Colour BMP::CountNewColour(const std::array<std::array<int, 3>, 3>& matrix, size_t i, size_t j) {
    std::array<int, 3> result_int = {0, 0, 0};
    std::array<std::array<int, 3>, 3> delta_x = {{{{-1, 0, 1}},
                                                 {{-1, 0, 1}},
                                                 {{-1, 0, 1}}}};
    std::array<std::array<int, 3>, 3> delta_y = {{{{-1 - 1, -1}},
                                                 {{0, 0, 0}},
                                                  {{1, 1, 1}}}};
    for (size_t k = 0; k < 3; ++k) {
        for (size_t t = 0; t < 3; ++t) {
            size_t needed_row = 0;
            size_t needed_col = 0;
            if (0 <= i + delta_y[k][t] && i + delta_y[k][t] < height) {
                needed_row = i + delta_y[k][t];
            } else if (height <= i + delta_y[k][t]) {
                needed_row = height - 1;
            }
            if (0 <= j + delta_x[k][t] && j + delta_x[k][t] < width) {
                needed_col = j + delta_x[k][t];
            } else if (width >= j + delta_x[k][t]) {
                needed_col = width - 1;
            }
            result_int[0] += matrix[k][t] * pixel_array.rows[needed_row].colours[needed_col].blue;
            result_int[1] += matrix[k][t] * pixel_array.rows[needed_row].colours[needed_col].green;
            result_int[2] += matrix[k][t] * pixel_array.rows[needed_row].colours[needed_col].red;
        }
    }
    std::array<uint8_t, 3> result_uint8_t = {0, 0, 0};
    for (size_t k = 0; k < 3; ++k) {
        result_uint8_t[k] = static_cast<uint8_t>(std::max(0, std::min(255, result_int[k])));
    }
    return Colour(result_uint8_t);
}

void BMP::ApplyMatrix(const std::array<std::array<int, 3>, 3> &matrix) {
    std::vector<ColourRow> new_rows(pixel_array.rows_number, ColourRow(pixel_array.rows_size));
    for (size_t i = 0; i < pixel_array.rows_number; ++i) {
        for (size_t j = 0; j < pixel_array.rows_size; ++j) {
            new_rows[i].colours[j] = CountNewColour(matrix, i, j);
        }
    }
    pixel_array.rows = std::move(new_rows);
}

Colour BMP::CountVertical(size_t sigma, size_t x0, size_t y0, double pi_constant, double exp_constant) {
    size_t min_y = std::max(static_cast<size_t>(0), y0 - 3 * sigma);
    size_t max_y = std::min(static_cast<size_t>(height) - 1, y0 + 3 * sigma);
    std::array<int, 3> result_int = {0, 0, 0};
    double koef_sum = 0;
    for (size_t i = min_y; i < y0; ++i) {
        double constant = pi_constant * std::powl(exp_constant, (y0 - i) * (y0 - i));
        koef_sum += constant;
        result_int[0] += pixel_array.rows[i].colours[x0].blue * constant;
        result_int[1] += pixel_array.rows[i].colours[x0].green * constant;
        result_int[2] += pixel_array.rows[i].colours[x0].red * constant;
    }
    for (size_t i = y0; i < max_y; ++i) {
        double constant = pi_constant * std::powl(exp_constant, (i - y0) * (i - y0));
        koef_sum += constant;
        result_int[0] += pixel_array.rows[i].colours[x0].blue * constant;
        result_int[1] += pixel_array.rows[i].colours[x0].green * constant;
        result_int[2] += pixel_array.rows[i].colours[x0].red * constant;
    }
    std::array<uint8_t, 3> result_uint8_t = {0, 0, 0};
    for (size_t k = 0; k < 3; ++k) {
        result_uint8_t[k] = static_cast<uint8_t>(std::max(0.0, std::min(255.0, result_int[k] / koef_sum)));
    }
    return Colour(result_uint8_t);
}

Colour BMP::CountHorizontal(size_t sigma, size_t x0, size_t y0, double pi_constant, double exp_constant) {
    size_t min_x = std::max(static_cast<size_t>(0), x0 - 3 * sigma);
    size_t max_x = std::min(static_cast<size_t>(width) - 1, x0 + 3 * sigma);
    std::array<int, 3> result_int = {0, 0, 0};
    double koef_sum = 0;
    for (size_t i = min_x; i < x0; ++i) {
        double constant = pi_constant * std::powl(exp_constant, (x0 - i) * (x0 - i));
        koef_sum += constant;
        result_int[0] += pixel_array.rows[y0].colours[i].blue * constant;
        result_int[1] += pixel_array.rows[y0].colours[i].green * constant;
        result_int[2] += pixel_array.rows[y0].colours[i].red * constant;
    }
    for (size_t i = x0; i < max_x; ++i) {
        double constant = pi_constant * std::powl(exp_constant, (i - x0) * (i - x0));
        koef_sum += constant;
        result_int[0] += pixel_array.rows[y0].colours[i].blue * constant;
        result_int[1] += pixel_array.rows[y0].colours[i].green * constant;
        result_int[2] += pixel_array.rows[y0].colours[i].red * constant;
    }
    std::array<uint8_t, 3> result_uint8_t = {0, 0, 0};
    for (size_t k = 0; k < 3; ++k) {
        result_uint8_t[k] = static_cast<uint8_t>(std::max(0.0, std::min(255.0, result_int[k] / koef_sum)));
    }
    return Colour(result_uint8_t);
}
