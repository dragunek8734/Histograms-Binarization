#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>

class Pixel {
    private:
        int R;
        
        int G;
        
        int B;
        
        int maxVal;

    public:
        Pixel()
            :R(0), G(0),B(0),maxVal(255){};

        Pixel(int r, int g, int b,int mVal)
            :R(r), G(g), B(b),maxVal(mVal){};
        

        ~Pixel() = default;

        int GetRed() const;
        int GetGreen() const;
        int GetBlue() const;

        void setPixel(int r, int g, int b);

};

class PPM {

    private:
        int width;
        int height;
        int pixelMaxval;
        std::vector<std::vector<Pixel>> map;

    public:
        PPM(){};

        PPM(int w, int h, int maxval)
            :width(w),height(h),pixelMaxval(maxval),map(w,std::vector<Pixel>(h,Pixel(0,0,0,pixelMaxval))){};


        Pixel& getPixel(int x, int y);
        const Pixel& getPixel(int x, int y) const;

        int getWidth() const;
        int getHeight() const;
        int getMaxVal() const;
        void LoadFromFile(const std::string& filename);
    
};

#endif // UTILS_H

