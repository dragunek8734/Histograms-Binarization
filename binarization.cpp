#include "histogram.h"
#include "utils.h"
#include <algorithm>
#include <vector>
#include <cmath>

PPM GrayScale(PPM &picture)
{

    int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    PPM *newpic = new PPM(w, h, maxval);

    for (int y = 0; y < picture.getHeight(); y++)
    {
        for (int x = 0; x < picture.getWidth(); x++)
        {
            Pixel &pixel = picture.getPixel(x, y);
            Pixel &newpix = newpic->getPixel(x, y);

            float gray = static_cast<float>(pixel.GetRed()) * 0.299 + static_cast<float>(pixel.GetGreen()) * 0.587 + static_cast<float>(pixel.GetBlue()) * 0.114;
            int normalized_gray = static_cast<int>(std::round(gray));

            newpix.setPixel(normalized_gray, normalized_gray, normalized_gray);
        }
    }

    PPM result = *newpic;
    delete newpic;

    return result;
}

PPM userThreshold(PPM &picture, int threshold)
{
    int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    int border = std::clamp(threshold, 0, picture.getMaxVal());

    PPM result = GrayScale(picture);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            if (gray <= border)
                pix.setPixel(0, 0, 0);
            else
                pix.setPixel(maxval, maxval, maxval);
        }
    }

    return result;
}

PPM PercentageBlack(PPM &picture, int prcnt)
{

    int threshold = std::clamp(prcnt, 15, 85);

    int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    PPM result = GrayScale(picture);

    std::vector<int> histogram(maxval + 1, 0);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            histogram[gray]++;
        }
    }

    int target_bl_pixels = std::round((static_cast<float>(threshold) / 100.0f) * (w * h));

    std::vector<int> CDF(maxval + 1, 0);

    int T = maxval;

    CDF[0] = histogram[0];

    for (int i = 1; i <= maxval; i++)
    {
        CDF[i] = CDF[i - 1] + histogram[i];
        if (CDF[i] >= target_bl_pixels)
        {
            T = i;
            break;
        }
    }

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            if (gray <= T)
                pix.setPixel(0, 0, 0);
            else
                pix.setPixel(maxval, maxval, maxval);
        }
    }

    return result;
}

PPM MeanIterative(PPM &picture)
{
    int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    PPM result = GrayScale(picture);

    std::vector<int> histogram(maxval + 1, 0);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            histogram[gray]++;
        }
    }

    long long sum = 0;

    for (int i = 0; i <= maxval; i++)
    {
        sum += static_cast<long long>(i) * static_cast<long long>(histogram[i]);
    }

    int T = static_cast<int>(std::round(static_cast<float>(sum) / static_cast<float>(w * h)));

    while (1)
    {
        long long sum1 = 0;
        long long pixnum1 = 0;

        long long sum2 = 0;
        long long pixnum2 = 0;

        for (int i = 0; i <= T; i++)
        {
            sum1 += static_cast<long long>(i) * static_cast<long long>(histogram[i]);
            pixnum1 += static_cast<long long>(histogram[i]);
        }

        for (int i = T + 1; i <= maxval; i++)
        {
            sum2 += static_cast<long long>(i) * static_cast<long long>(histogram[i]);
            pixnum2 += static_cast<long long>(histogram[i]);
        }

        double mean1 = pixnum1 > 0 ? static_cast<double>(sum1) / pixnum1 : 0.0;
        double mean2 = pixnum2 > 0 ? static_cast<double>(sum2) / pixnum2 : static_cast<double>(maxval);

        int T_new = static_cast<int>(std::round((mean1 + mean2) / 2.0));

        if (std::abs(T - T_new) < 1)
        {
            T = T_new;
            break;
        }
        else
            T = T_new;
    }

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            if (gray <= T)
                pix.setPixel(0, 0, 0);
            else
                pix.setPixel(maxval, maxval, maxval);
        }
    }

    return result;
}

PPM EntropySelection(PPM &picture)
{
    int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    PPM result = GrayScale(picture);

    std::vector<int> histogram(maxval + 1, 0);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            histogram[gray]++;
        }
    }

    std::vector<float> probability(maxval + 1, 0);

    for (int i = 0; i <= maxval; i++)
    {
        probability[i] = static_cast<float>(histogram[i]) / static_cast<float>(w * h);
    }

    std::vector<float> entropy(maxval + 1, 0);

    float max_entropy = -std::numeric_limits<float>::infinity();
    int best_T = 0;

    for (int i = 0; i <= maxval; i++)
    {
        float entropy_a = 0.0f;
        float entropy_b = 0.0f;

        float P_A = 0.0f;
        float P_B = 0.0f;

        for (int a = 0; a <= i; a++)
        {
            P_A += probability[a];
        }

        P_B = 1.0f - P_A;

        if (P_A > 0.0f)
        {
            for (int a = 0; a <= i; a++)
            {
                if (probability[a] == 0)
                    continue;

                entropy_a -= (probability[a] / P_A) * (std::log2(probability[a] / P_A));
            }
        }

        if (P_B > 0.0f)
        {
            for (int b = i + 1; b <= maxval; b++)
            {
                if (probability[b] == 0)
                    continue;

                entropy_b -= (probability[b] / P_B) * (std::log2(probability[b] / P_B));
            }
        }

        entropy[i] = entropy_a + entropy_b;

        if (entropy[i] > max_entropy)
        {
            best_T = i;
            max_entropy = entropy[i];
        }
            
    }

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            if (gray <= best_T)
                pix.setPixel(0, 0, 0);
            else
                pix.setPixel(maxval, maxval, maxval);
        }
    }

    return result;
}

PPM MinimumError(PPM &picture)
{
     int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    PPM result = GrayScale(picture);

    std::vector<int> histogram(maxval + 1, 0);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            histogram[gray]++;
        }
    }

    std::vector<float> probability(maxval + 1, 0);

    for (int i = 0; i <= maxval; i++)
    {
        probability[i] = static_cast<float>(histogram[i]) / static_cast<float>(w * h);
    }


    std::vector<float> error_cost(maxval+1,0);


    int best_T = 0;
    float minError = std::numeric_limits<float>::infinity();


    for(int i = 0; i < maxval - 1; i++)
    {
        float P1 = 0.0f;
        float P2 = 0.0f;
        float mean1 = 0.0f;
        float mean2 = 0.0f;
        float sigma21 = 0.0f;
        float sigma22 = 0.0f;


        for(int a = 0; a <= i; a++)
        {
            P1 += probability[a];
        }

        P2 = 1.0f - P1;
        

        float sum1 = 0.0f, sum2 = 0.0f;

        for(int a = 0; a <= i; a++)
        {
            sum1 += static_cast<float>(a) * probability[a];
        }

        for(int b = i + 1; b <=maxval; b++)
        {
            sum2 += static_cast<float>(b) * probability[b];
        }

        mean1 = sum1 / P1;
        mean2 = sum2 / P2;


        sum1 = 0.0f;
        sum2 = 0.0f;
        for(int a = 0; a <= i; a++)
        {
            sum1 += std::pow((static_cast<float>(a) - mean1),2) * probability[a];
        }

        for(int b = i + 1; b <=maxval; b++)
        {
            sum2 += std::pow((static_cast<float>(b) - mean2),2) * probability[b];
        }

        sigma21 = sum1 / P1;
        sigma22 = sum2 / P2;

        if(sigma21 <= 0.0f)
            sigma21 = 1e-10;

        if(sigma22 <= 0.0f)
            sigma22 = 1e-10;
        

        float sigma1 = std::sqrt(sigma21);
        float sigma2 = std::sqrt(sigma22);


        error_cost[i] = 1.0f + 2.0f*(P1 * std::log(sigma1) + P2 * std::log(sigma2)) - 2.0f*( P1 * std::log(P1) + P2 * std::log(P2));

        if(error_cost[i] < minError)
        {
            best_T = i;
            minError = error_cost[i];
        }
            

    }

     for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            if (gray <= best_T)
                pix.setPixel(0, 0, 0);
            else
                pix.setPixel(maxval, maxval, maxval);
        }
    }

    return result;


}


// dokonczyc
PPM FuzyyMinimum(PPM &picture)
{
      int maxval = picture.getMaxVal();
    int w = picture.getWidth();
    int h = picture.getHeight();

    PPM result = GrayScale(picture);

    std::vector<int> histogram(maxval + 1, 0);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            histogram[gray]++;
        }
    }

    std::vector<float> probability(maxval + 1, 0);

    for (int i = 0; i <= maxval; i++)
    {
        probability[i] = static_cast<float>(histogram[i]) / static_cast<float>(w * h);
    }


    std::vector<float> error_cost(maxval+1,0);


    int best_T = 0;
    float minError = std::numeric_limits<float>::infinity();


    for(int i = 0; i < maxval - 1; i++)
    {
        float P1 = 0.0f;
        float P2 = 0.0f;
        float mean1 = 0.0f;
        float mean2 = 0.0f;
        float sigma21 = 0.0f;
        float sigma22 = 0.0f;


        for(int a = 0; a <= i; a++)
        {
            P1 += probability[a];
        }

       if(P1  < 1e-6 || P2 < 1e-6)
        continue;
    


        P2 = 1.0f - P1;

        

        float sum1 = 0.0f, sum2 = 0.0f;

        for(int a = 0; a <= i; a++)
        {
            sum1 += static_cast<float>(a) * probability[a];
        }

        for(int b = i + 1; b <=maxval; b++)
        {
            sum2 += static_cast<float>(b) * probability[b];
        }

        mean1 = sum1 / P1;
        mean2 = sum2 / P2;


        sum1 = 0.0f;
        sum2 = 0.0f;
        for(int a = 0; a <= i; a++)
        {
            sum1 += std::pow((static_cast<float>(a) - mean1),2) * probability[a];
        }

        for(int b = i + 1; b <=maxval; b++)
        {
            sum2 += std::pow((static_cast<float>(b) - mean2),2) * probability[b];
        }

        sigma21 = sum1 / P1;
        sigma22 = sum2 / P2;

        if(sigma21 <= 0.0f)
            sigma21 = 1e-10;

        if(sigma22 <= 0.0f)
            sigma22 = 1e-10;
        

        float sigma1 = std::sqrt(sigma21);
        float sigma2 = std::sqrt(sigma22);

        

        float minerror = std::numeric_limits<float>::infinity();
        int bestT = 0;
        float fuzzy_error = 0.0f;

        for(int n = 0; n <= maxval; n++)
        {
            if(probability[n] == 0.0f)
                continue;
            
            
            //gauss
            double d1 = std::pow(n - mean1,2) / (2 * sigma21);
            double d2 = std::pow(n - mean2,2) / (2 * sigma22);

            double u_background = (P1 / (sigma1 + 1e-10)) * std::exp(-d1);
            double u_object = (P2 / (sigma2 + 1e-10)) * std::exp(-d2);

            double sum = u_background + u_object;

            if(sum > 0.0)
            {
                u_background = u_background/sum;
                u_object = u_object/sum;
            }
            else
            {
                u_background = u_object = 0.5;
            }

            // Shannon entropy

            double h_bg = (u_background > 1e-10) ? u_background * std::log(u_background) : 0.0;
            double h_obj = (u_object > 1e-10) ? u_object * std::log(u_object) : 0.0;

            fuzzy_error -= probability[n] * (h_bg + h_obj);

        }

        if( fuzzy_error < minError)
        {
            minError = fuzzy_error;
            best_T = i;
        }
    }

       for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Pixel &pix = result.getPixel(x, y);
            int gray = pix.GetRed();

            if (gray <= best_T)
                pix.setPixel(0, 0, 0);
            else
                pix.setPixel(maxval, maxval, maxval);
        }
    }

    return result;

}
