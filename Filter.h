#ifndef OIMP_PROJECT_FILTER_H
#define OIMP_PROJECT_FILTER_H

#include <string>
#include <vector>
#include "BMP.h"

#pragma once

class Filter {
public:
    std::vector<int> params;

    Filter(const std::vector<int>& params);
    Filter() = default;
    virtual void ApplyFilter(BMP& image) = 0;
    virtual ~Filter() = default;
};

class Crop : public Filter {
public:
    Crop(const std::vector<int>& params);
    void ApplyFilter(BMP& image);
};

class Gs : public Filter {
public:
    Gs() = default;
    Gs(const std::vector<int>& params);
    void ApplyFilter(BMP& image);
};

class Neg : public Filter {
public:
    Neg() = default;
    Neg(const std::vector<int>& params);
    void ApplyFilter(BMP& image);
};

class Sharp : public Filter {
public:
    Sharp() = default;
    Sharp(const std::vector<int>& params);
    void ApplyFilter(BMP& image);
};

class Edge : public Filter {
public:
    Edge(const std::vector<int>& params);
    void ApplyFilter(BMP& image);
};

class Blur : public Filter {
public:
    Blur(const std::vector<int>& params);
    void ApplyFilter(BMP& image);
};

class Acos : public Filter {
public:
    Acos() = default;
    Acos(const std::vector<int>& params);
    void ApplyFilter(BMP& image);
};

#endif //OIMP_PROJECT_FILTER_H