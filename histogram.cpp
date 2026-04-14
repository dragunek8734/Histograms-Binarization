#include "histogram.h"
#include "utils.h"
#include <algorithm>
#include <cmath>

Histogram::Histogram( PPM &picture){
    updateHist(picture);
}


void Histogram::updateHist(PPM &picture)
{
    int pixmaxval = picture.getMaxVal();

    int w = picture.getWidth();
    int h = picture.getHeight();

    hist_red.assign(pixmaxval+1,0);
    hist_green.assign(pixmaxval+1,0);
    hist_blue.assign(pixmaxval+1,0);

    const Pixel &zero = picture.getPixel(0,0);

    max_r = zero.GetRed();
    min_r = zero.GetRed();

    max_g = zero.GetGreen();
    min_g = zero.GetGreen();

    max_b = zero.GetBlue();
    min_b = zero.GetBlue();


    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            const Pixel &pix = picture.getPixel(x,y);
            int red = pix.GetRed();
            int green = pix.GetGreen();
            int blue = pix.GetBlue();

            min_r = std::min(red,min_r);
            max_r = std::max(red,max_r);

            min_g = std::min(green,min_g);
            max_g = std::max(green,max_g);

            min_b = std::min(blue,min_b);
            max_b = std::max(blue,max_b);

            incrementRVal(red);
            incrementGVal(green);
            incrementBVal(blue);
        }
    }
}


void Histogram::stretch(PPM &picture) {

    // Calculate histogram first
    updateHist(picture);
    
    int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();




    for( int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            Pixel &pix = picture.getPixel(x,y);
            int red = pix.GetRed();
            int green = pix.GetGreen();
            int blue = pix.GetBlue();

            int new_r, new_g, new_b;

            if(max_r != min_r)
            {
                new_r = (red - min_r) * maxval / (max_r - min_r);
                new_r = std::clamp(new_r,0,maxval);
            }
            else
                new_r = red;
            
            if(max_g != min_g)
            {
                new_g = (green - min_g) * maxval / (max_g - min_g);
                new_g = std::clamp(new_g,0,maxval);
            }
            else
                new_g = green;

            if(max_b != min_b)
            {
                new_b = (blue - min_b) * maxval / (max_b - min_b);
                new_b = std::clamp(new_b,0,maxval);
            }
            else
                new_b = blue;


            pix.setPixel(new_r,new_g,new_b);
        }
    }

    updateHist(picture);

}

void Histogram::normalize(PPM &picture) {
    
    // Calculate histogram first
    updateHist(picture);
    
    int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    std::vector<int> CDF_R(maxval+1,0);
    std::vector<int> CDF_G(maxval+1,0);
    std::vector<int> CDF_B(maxval+1,0);

    CDF_R[0] = hist_red[0];
    CDF_G[0] = hist_green[0];
    CDF_B[0] = hist_blue[0];

    for(int i = 1; i <=maxval; i++)
    {
        CDF_R[i] = CDF_R[i-1] +  hist_red[i];
        CDF_G[i] = CDF_G[i-1] +  hist_green[i];
        CDF_B[i] = CDF_B[i-1] +  hist_blue[i];
    }

    int cdf_r_min = 0, cdf_g_min = 0, cdf_b_min = 0;

    for(int i = 0; i <=maxval;i++)
    {
        if(CDF_R[i] != 0)
        {
            cdf_r_min = CDF_R[i];
            break;
        }
    }

    for(int i = 0; i <=maxval;i++)
    {
        if(CDF_G[i] != 0)
        {
            cdf_g_min = CDF_G[i];
            break;
        }
    }

    for(int i = 0; i <=maxval;i++)
    {
        if(CDF_B[i] != 0)
        {
            cdf_b_min = CDF_B[i];
            break;
        }
    }


    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            Pixel &pix = picture.getPixel(x,y);
            int red = pix.GetRed();
            int green = pix.GetGreen();
            int blue = pix.GetBlue();


            int new_r,new_g,new_b;


            if(w*h == cdf_r_min)
            {
                new_r = red;
            }
            else
            {
                new_r = std::round((float)(CDF_R[red] - cdf_r_min) / (w * h - cdf_r_min) * maxval);
            }

            if(w*h == cdf_g_min)
            {
                new_g = green;
            }
            else
            {
                new_g = std::round((float)(CDF_G[green] - cdf_g_min) / (w * h - cdf_g_min) * maxval);
            }

            if(w*h == cdf_b_min)
            {
                new_b = blue;
            }
            else
            {
                new_b = std::round((float)(CDF_B[blue] - cdf_b_min) / (w * h - cdf_b_min) * maxval);
            }
            
            
            

            new_r = std::clamp(new_r,0,maxval);
            new_g = std::clamp(new_g,0,maxval);
            new_b = std::clamp(new_b,0,maxval);

            pix.setPixel(new_r,new_g,new_b);
        }
    }

    updateHist(picture);

}