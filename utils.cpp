#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include "utils.h"

#include <direct.h>
//                                                          PIXEL
int Pixel::GetRed() const
{
    return R;
};
int Pixel::GetGreen() const
{
    return G;
};
int Pixel::GetBlue() const
{
    return B;
};

void Pixel::setPixel(int r, int g, int b)
{
    if (r < 0 || r > maxVal ||
        g < 0 || g > maxVal ||
        b < 0 || b > maxVal)
        throw std::out_of_range("One pixel value is out of range.");

    R = r;
    G = g;
    B = b;
};

// auto removeComment = [](std::string &line)
// {
//     size_t pos = line.find('#');
//     if (pos != std::string::npos)
//         line = line.substr(0, pos);
// };

//                                                  PPM3

int PPM::getWidth() const
{
    return width;
}

int PPM::getHeight() const
{
    return height;
}

int PPM::getMaxVal() const
{
    return pixelMaxval;
}

Pixel& PPM::getPixel(int x, int y)
{
    return map[y][x];
}

const Pixel &PPM::getPixel(int x, int y) const
{
    // if (x < 0 || x >= width || y < 0 || y >= height)
    // {
    //     std::cerr << "not exists" << std::endl;
    //     throw std::out_of_range("Pixel out of range");
    // }

    return map[y][x]; // marked
};

void PPM::LoadFromFile(const std::string &filename)
{
    // Add this debug code
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        std::cout << "Current working directory: " << cwd << std::endl;
    }

    std::cout << "Trying to open: " << filename << std::endl;

    std::ifstream file(filename);

    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    std::string line;
    int format;
    std::getline(file, line);

    if (line != "P3" && line != "P6")
        throw std::runtime_error("This file is not in PM3/PM6 format");

    format = (line == "P3") ? 0 : 1;

    if (format == 0)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file: " + filename);

        // wczytywanie pliku do bufora
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        std::string content(buffer.begin(), buffer.end());
        std::istringstream iss(content);

        std::string token;
        auto nextToken = [&]() -> std::string
        {
            while (iss >> token)
            {
                if (token[0] == '#')
                {
                    std::string dummy;
                    std::getline(iss, dummy); // koment
                    continue;
                }
                return token;
            }
            return "";
        };

        // format
        std::string formatStr = nextToken();
        if (formatStr != "P3")
            throw std::runtime_error("Not a P3 format");

        // wymiary
        width = std::stoi(nextToken());
        height = std::stoi(nextToken());

        pixelMaxval = std::stoi(nextToken());

        map.assign(height, std::vector<Pixel>(width, Pixel(0, 0, 0, pixelMaxval))); // marked switch width height

        int totalPixels = width * height * 3;
        std::vector<int> pixelData;
        pixelData.reserve(totalPixels);

        while (pixelData.size() < totalPixels)
        {
            std::string valStr = nextToken();
            if (valStr.empty())
                break;
            pixelData.push_back(std::stoi(valStr));
        }

        if (pixelData.size() != totalPixels)
            throw std::runtime_error("Incorrect number of pixel values");

        int idx = 0;
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int r = pixelData[idx++];
                int g = pixelData[idx++];
                int b = pixelData[idx++];
                map[y][x].setPixel(r, g, b); // marked
            }
        }
    }
    else // case PPM6
    {
        std::ifstream binFile(filename, std::ios::binary);
        if (!binFile.is_open())
            throw std::runtime_error("Cannot open file in binary mode: " + filename);

        std::string token;
        auto nextToken = [&](std::ifstream &f) -> std::string
        {
            while (f >> token)
            {
                if (token[0] == '#')
                {
                    std::string dummy;
                    std::getline(f, dummy);
                    continue;
                }
                return token;
            }
            return "";
        };

        std::string magic = nextToken(binFile);
        if (magic != "P6")
            throw std::runtime_error("Not P6");

        width = std::stoi(nextToken(binFile));
        height = std::stoi(nextToken(binFile));
        pixelMaxval = std::stoi(nextToken(binFile));

        binFile.get();
        if (width <= 0 || height <= 0)
            throw std::runtime_error("Invalid image dimensions");

        map.assign(height, std::vector<Pixel>(width, Pixel(0, 0, 0, pixelMaxval))); // marked

        // binFile.get(); 

        binFile >> std::ws;

        size_t bytes_per_channel = (pixelMaxval > 255) ? 2 : 1;
        const size_t dataSize = static_cast<size_t>(width) * height * 3 * bytes_per_channel;
        std::vector<uint8_t> pixelData(dataSize);

        binFile.read(reinterpret_cast<char *>(pixelData.data()), dataSize);
        std::streamsize actually = binFile.gcount();
        std::cout << "actuallyRead=" << actually << "\n";
        size_t dumpN = std::min<size_t>(5000000, (size_t)actually);
        // for (size_t i=actually - 1;i> 50000;--i) printf("%02X ", pixelData[i]);
        // printf("\n");

        // std::streamsize actually = binFile.gcount();
        if (actually < static_cast<std::streamsize>(dataSize))
            throw std::runtime_error("File shorter than expected");
        if (actually > static_cast<std::streamsize>(dataSize))
            throw std::runtime_error("File shorter than expected");

        std::cout << "width=" << width << " height=" << height
                  << " dataSize=" << pixelData.size() << std::endl;

        size_t idx = 0;
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                if (idx + (bytes_per_channel == 1 ? 3 : 6) > pixelData.size())
                {
                    throw std::runtime_error("Index overflow at x=" + std::to_string(x) +
                                             " y=" + std::to_string(y));
                }
                int r, g, b;
                if (bytes_per_channel == 1)
                {
                    r = pixelData[idx++];
                    g = pixelData[idx++];
                    b = pixelData[idx++];
                }
                else
                {
                    r = (pixelData[idx] << 8) | pixelData[idx + 1];
                    g = (pixelData[idx + 2] << 8) | pixelData[idx + 3];
                    b = (pixelData[idx + 4] << 8) | pixelData[idx + 5];
                    idx += 6;
                    r = (r * 255) / pixelMaxval;
                    g = (g * 255) / pixelMaxval;
                    b = (b * 255) / pixelMaxval;
                }
                r = std::clamp(r, 0, 255);
                g = std::clamp(g, 0, 255);
                b = std::clamp(b, 0, 255);
                map[y][x].setPixel(r, g, b); // marked
            }
        }
    }
};
