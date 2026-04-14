#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "utils.h"
#include <vector>

class Histogram
{
private:
    std::vector<int> hist_red;
    int max_r;
    int min_r;
    std::vector<int> hist_green;
    int max_g;
    int min_g;
    std::vector<int> hist_blue;
    int max_b;
    int min_b;

public:
    Histogram( PPM &picture);
    Histogram() : max_r(0), max_g(0), max_b(0), min_r(0), min_g(0), min_b(0)
    {
        hist_red.assign(256, 0);
        hist_green.assign(256, 0);
        hist_blue.assign(256, 0);
    }

    ~Histogram() = default;

    // Getters

    int getMaxR() const
    {
        return max_r;
    }

    int getMinR() const
    {
        return min_r;
    }

    int getMaxG() const
    {
        return max_g;
    }

    int getMinG() const
    {
        return min_g;
    }

    int getMaxB() const
    {
        return max_b;
    }

    int getMinB() const
    {
        return min_b;
    }

    int getRCount(int val) const
    {
        return hist_red[val];
    }

    int getGCount(int val) const
    {
        return hist_green[val];
    }

    int getBCount(int val) const
    {
        return hist_blue[val];
    }

    const std::vector<int> &getHistRed() const
    {
        return hist_red;
    }

    const std::vector<int> &getHistGreen() const
    {
        return hist_green;
    }

    const std::vector<int> &getHistBlue() const
    {
        return hist_blue;
    }

    int getTotalRed() const
    {
        int sum = 0;

        for (int v : hist_red)
            sum += v;

        return sum;
    }

    int getTotalGreen() const
    {
        int sum = 0;

        for (int v : hist_green)
            sum += v;

        return sum;
    }

    int getTotalBlue() const
    {
        int sum = 0;

        for (int v : hist_blue)
            sum += v;

        return sum;
    }

    // setters

    void setMaxR(int maxval)
    {
        if (maxval > 0)
        {
            max_r = maxval;
            hist_red.resize(maxval + 1, 0);
        }
    }

    void setMaxG(int maxval)
    {
        if (maxval > 0)
        {
            max_g = maxval;
            hist_green.resize(maxval + 1, 0);
        }
    }

    void setMaxB(int maxval)
    {
        if (maxval > 0)
        {
            max_b = maxval;
            hist_blue.resize(maxval + 1, 0);
        }
    }

    void setMinR(int minval)
    {
        if (minval >= 0 && minval <= max_r)
        {
            min_r = minval;
        }
    }

    void setMinG(int minval)
    {
        if (minval >= 0 && minval <= max_g)
        {
            min_g = minval;
        }
    }

    void setMinB(int minval)
    {
        if (minval >= 0 && minval <= max_b)
        {
            min_b = minval;
        }
    }

    void incrementRVal(int value)
    {
        if (value < 1 || value > max_r)
            return;

        ++hist_red[value];
    }

    void incrementGVal(int value)
    {
        if (value < 1 || value > max_g)
            return;

        ++hist_green[value];
    }

    void incrementBVal(int value)
    {
        if (value < 1 || value > max_b)
            return;

        ++hist_blue[value];
    }

    void clear()
    {
        std::fill(hist_red.begin(), hist_red.end(), 0);
        std::fill(hist_green.begin(), hist_green.end(), 0);
        std::fill(hist_blue.begin(), hist_blue.end(), 0);
    }

    void normalize(PPM &picture);

    void stretch(PPM &picture);

    void updateHist(PPM &picture);


};

#endif // HISTOGRAM_H