#include "Filter.h"
#include <iostream>
#include <cmath>

const double pi = M_PI;

Filter::Filter(const std::vector<int>& params) {
    this->params = params;
}

Crop::Crop(const std::vector<int>& params) : Filter(params) {
    this->params = params;
}

void Crop::ApplyFilter(BMP& image) {
    int new_width = params[0];
    int new_height = params[1];
    if (new_width <= 0 || new_height <= 0) {
        throw std::invalid_argument("Width and height can't be less or equal to 0");
    }
    image.SetHeight(new_height);
    image.SetWidth(new_width);
}

Gs::Gs(const std::vector<int>& params) : Filter(params) {
    this->params = params;
}

void Gs::ApplyFilter(BMP& image) {
    for (auto &row: image.pixel_array.rows) {
        for (auto &colour: row.colours) {
            int red = colour.red;
            int green = colour.green;
            int blue = colour.blue;
            colour.blue = colour.green = colour.red = static_cast<int>(0.299 * red + 0.587 * green + 0.114 * blue);
        }
    }
}

Neg::Neg(const std::vector<int>& params) : Filter(params) {
    this->params = params;
}

void Neg::ApplyFilter(BMP& image) {
    for (auto &row: image.pixel_array.rows) {
        for (auto &colour: row.colours) {
            colour.blue = 255 - colour.blue;
            colour.green = 255 - colour.green;
            colour.red = 255 - colour.red;
        }
    }
}

Sharp::Sharp(const std::vector<int>& params) : Filter(params) {
    this->params = params;
}

void Sharp::ApplyFilter(BMP& image) {
    std::array<std::array<int, 3>, 3> matrix = {{{{0,  -1, 0}},
                                                 {{-1, 5,  -1}},
                                                 {{0,  -1, 0}}}};
    image.ApplyMatrix(matrix);
}

Edge::Edge(const std::vector<int>& params) : Filter(params) {
    this->params = params;
}

void Edge::ApplyFilter(BMP& image) {
    int threshold = params[0];
    Gs gs;
    gs.ApplyFilter(image);
    std::array<std::array<int, 3>, 3> matrix = {{{{0,  -1, 0}},
                                                 {{-1, 4,  -1}},
                                                 {{0,  -1, 0}}}};
    image.ApplyMatrix(matrix);
    for (size_t i = 0; i < image.height; ++i) {
        for (size_t j = 0; j < image.width; ++j) {
            if (image.pixel_array.rows[i].colours[j].blue > threshold) {
                image.pixel_array.rows[i].colours[j].blue = image.pixel_array.rows[i].colours[j].green =
                    image.pixel_array.rows[i].colours[j].red = 255;
            } else {
                image.pixel_array.rows[i].colours[j].blue = image.pixel_array.rows[i].colours[j].green =
                    image.pixel_array.rows[i].colours[j].red = 0;
            }
        }
    }
}

Blur::Blur(const std::vector<int>& params) : Filter(params) {
    this->params = params;
}

void Blur::ApplyFilter(BMP& image) {
    int sigma = params[0];
    double pi_constant = 1.0 / sqrt((2 * pi) * sigma);
    double exp_constant = std::exp(-1.0 / (2 * sigma * sigma));
    std::vector<ColourRow> new_rows(image.pixel_array.rows_number, ColourRow(image.pixel_array.rows_size));
    for (size_t i = 0; i < image.height; ++i) {
        for (size_t j = 0; j < image.width; ++j) {
            new_rows[i].colours[j] = image.CountVertical(sigma, j, i, pi_constant, exp_constant);
        }
    }
    image.pixel_array.rows = std::move(new_rows);
    std::vector<ColourRow> new_rows1(image.pixel_array.rows_number, ColourRow(image.pixel_array.rows_size));
    for (size_t i = 0; i < image.height; ++i) {
        for (size_t j = 0; j < image.width; ++j) {
            new_rows1[i].colours[j] = image.CountHorizontal(sigma, j, i, pi_constant, exp_constant);
        }
    }
    image.pixel_array.rows = std::move(new_rows1);
}

Acos::Acos(const std::vector<int>& params) : Filter(params) {
    this->params = params;
}

void Acos::ApplyFilter(BMP& image) {
    for (auto &row: image.pixel_array.rows) {
        for (auto &colour: row.colours) {
            colour.blue = acos(colour.blue / 255.0) * 255;
            colour.green = acos(colour.green / 255.0) * 255;
            colour.red = acos(colour.green / 255.0) * 255;
        }
    }
}
