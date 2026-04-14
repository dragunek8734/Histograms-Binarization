#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "histogram.h"
#include "utils.h"

// Forward declarations for binarization functions
PPM GrayScale(PPM &picture);
PPM userThreshold(PPM &picture, int threshold);
PPM PercentageBlack(PPM &picture, int prcnt);
PPM MeanIterative(PPM &picture);
PPM EntropySelection(PPM &picture);
PPM MinimumError(PPM &picture);
PPM FuzyyMinimum(PPM &picture);

// Helper function to convert PPM to Mat for display
cv::Mat PPMtoMat(const PPM &picture, int height, int width)
{
    cv::Mat result(height, width, CV_8UC3);
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Pixel ppm_pixel = picture.getPixel(x, y);
            int r = ppm_pixel.GetRed();
            int g = ppm_pixel.GetGreen();
            int b = ppm_pixel.GetBlue();
            
            // Convert RGB to BGR for OpenCV
            result.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r);
        }
    }
    
    return result;
}

int main()
{
    // Load image using OpenCV
    std::string imagePath;
    std::cout << "========== HISTOGRAM & BINARIZATION PROGRAM ==========" << std::endl;
    std::cout << "Enter path to image file: ";
    std::getline(std::cin, imagePath);

    if (imagePath.empty())
    {
        std::cerr << "Error: Path cannot be empty!" << std::endl;
        return -1;
    }

    cv::Mat cvImage = cv::imread(imagePath);

    if (cvImage.empty())
    {
        std::cerr << "Error: Cannot load image from: " << imagePath << std::endl;
        std::cerr << "Check the path and ensure the file exists." << std::endl;
        return -1;
    }

    std::cout << "\nImage loaded successfully: " << imagePath << std::endl;
    std::cout << "Size: " << cvImage.cols << "x" << cvImage.rows << std::endl;

    // Convert BGR (OpenCV) to RGB and create PPM object
    cv::Mat rgbImage;
    cv::cvtColor(cvImage, rgbImage, cv::COLOR_BGR2RGB);

    int width = rgbImage.cols;
    int height = rgbImage.rows;
    int maxVal = 255;

    PPM picture(width, height, maxVal);

    // Copy pixels from OpenCV Mat to PPM
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            cv::Vec3b pixel = rgbImage.at<cv::Vec3b>(y, x);
            picture.getPixel(x, y).setPixel(pixel[0], pixel[1], pixel[2]);
        }
    }

    std::cout << "Image converted to PPM format" << std::endl;

    // Display original image
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", cvImage);
    std::cout << "\nOriginal image window opened and will remain visible." << std::endl;

    // Main menu
    bool running = true;

    while (running)
    {
        // Keep windows responsive - allow window interaction
        cv::waitKey(10);
        
        std::cout << "\n===== HISTOGRAM & BINARIZATION MENU =====" << std::endl;
        std::cout << "1. Load new image" << std::endl;
        std::cout << "2. Calculate histogram" << std::endl;
        std::cout << "3. Display histogram information" << std::endl;
        std::cout << "4. Perform histogram stretch" << std::endl;
        std::cout << "5. Perform histogram equalization" << std::endl;
        std::cout << "6. Convert to grayscale" << std::endl;
        std::cout << "7. Binarize with user threshold" << std::endl;
        std::cout << "8. Binarize with percentage black" << std::endl;
        std::cout << "9. Compare all binarization methods" << std::endl;
        std::cout << "10. Close all result windows" << std::endl;
        std::cout << "11. Exit" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Select option: ";

        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            // Load new image
            std::cout << "Enter image path: ";
            std::cin.ignore();
            std::getline(std::cin, imagePath);

            cvImage = cv::imread(imagePath);

            if (cvImage.empty())
            {
                std::cerr << "Error: Cannot load image from: " << imagePath << std::endl;
                break;
            }

            // Convert BGR -> RGB
            cv::cvtColor(cvImage, rgbImage, cv::COLOR_BGR2RGB);

            width = rgbImage.cols;
            height = rgbImage.rows;
            picture = PPM(width, height, maxVal);

            // Copy pixels
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    cv::Vec3b pixel = rgbImage.at<cv::Vec3b>(y, x);
                    picture.getPixel(x, y).setPixel(pixel[0], pixel[1], pixel[2]);
                }
            }

            // Update original image window
            cv::imshow("Original Image", cvImage);
            
            std::cout << "Image loaded: " << imagePath << std::endl;
            std::cout << "Size: " << width << "x" << height << std::endl;
            break;
        }

        case 2:
        {
            // Calculate histogram
            Histogram histogram(picture);
            std::cout << "\nHistogram calculated successfully!" << std::endl;
            std::cout << "RGB Red range: [" << histogram.getMinR() << ", " << histogram.getMaxR() << "]" << std::endl;
            std::cout << "RGB Green range: [" << histogram.getMinG() << ", " << histogram.getMaxG() << "]" << std::endl;
            std::cout << "RGB Blue range: [" << histogram.getMinB() << ", " << histogram.getMaxB() << "]" << std::endl;
            break;
        }

        case 3:
        {
            // Display histogram information
            Histogram histogram(picture);

            std::cout << "\n===== HISTOGRAM INFORMATION =====" << std::endl;
            std::cout << "RED Channel:" << std::endl;
            std::cout << "  Min: " << histogram.getMinR() << ", Max: " << histogram.getMaxR() << std::endl;
            std::cout << "  Total pixels: " << histogram.getTotalRed() << std::endl;

            std::cout << "GREEN Channel:" << std::endl;
            std::cout << "  Min: " << histogram.getMinG() << ", Max: " << histogram.getMaxG() << std::endl;
            std::cout << "  Total pixels: " << histogram.getTotalGreen() << std::endl;

            std::cout << "BLUE Channel:" << std::endl;
            std::cout << "  Min: " << histogram.getMinB() << ", Max: " << histogram.getMaxB() << std::endl;
            std::cout << "  Total pixels: " << histogram.getTotalBlue() << std::endl;
            std::cout << "================================" << std::endl;

            break;
        }

        case 4:
        {
            // Histogram stretch
            PPM stretchedPicture = picture;
            Histogram histogram(stretchedPicture);
            std::cout << "Performing histogram stretch..." << std::endl;
            histogram.stretch(stretchedPicture);
            std::cout << "Histogram stretch completed!" << std::endl;
            
            // Display result in new window
            cv::Mat resultImage = PPMtoMat(stretchedPicture, height, width);
            cv::namedWindow("Histogram Stretch Result", cv::WINDOW_NORMAL);
            cv::imshow("Histogram Stretch Result", resultImage);
            cv::waitKey(1);
            break;
        }

        case 5:
        {
            // Histogram equalization
            PPM normalizedPicture = picture;
            Histogram histogram(normalizedPicture);
            std::cout << "Performing histogram equalization..." << std::endl;
            histogram.normalize(normalizedPicture);
            std::cout << "Histogram equalization completed!" << std::endl;
            
            // Display result in new window
            cv::Mat resultImage = PPMtoMat(normalizedPicture, height, width);
            cv::namedWindow("Histogram Equalization Result", cv::WINDOW_NORMAL);
            cv::imshow("Histogram Equalization Result", resultImage);
            cv::waitKey(1);
            break;
        }

        case 6:
        {
            // Grayscale conversion
            PPM grayPicture = GrayScale(picture);
            std::cout << "Grayscale conversion completed!" << std::endl;
            
            // Display result in new window
            cv::Mat resultImage = PPMtoMat(grayPicture, height, width);
            cv::namedWindow("Grayscale Result", cv::WINDOW_NORMAL);
            cv::imshow("Grayscale Result", resultImage);
            cv::waitKey(1);
            break;
        }

        case 7:
        {
            // Binarize with user threshold
            std::cout << "Enter threshold value (0-255): ";
            int threshold;
            std::cin >> threshold;
            
            PPM binaryPicture = userThreshold(picture, threshold);
            std::cout << "Binarization with threshold " << threshold << " completed!" << std::endl;
            
            // Display result in new window
            cv::Mat resultImage = PPMtoMat(binaryPicture, height, width);
            cv::namedWindow("Binary Result (Threshold)", cv::WINDOW_NORMAL);
            cv::imshow("Binary Result (Threshold)", resultImage);
            cv::waitKey(1);
            break;
        }

        case 8:
        {
            // Binarize with percentage black
            std::cout << "Enter percentage of black pixels (15-85): ";
            int percentage;
            std::cin >> percentage;
            
            PPM binaryPicture = PercentageBlack(picture, percentage);
            std::cout << "Binarization with " << percentage << "% black completed!" << std::endl;
            
            // Display result in new window
            cv::Mat resultImage = PPMtoMat(binaryPicture, height, width);
            cv::namedWindow("Binary Result (Percentage)", cv::WINDOW_NORMAL);
            cv::imshow("Binary Result (Percentage)", resultImage);
            cv::waitKey(1);
            break;
        }

        case 9:
        {
            // Get current working directory
            std::string output_dir = std::filesystem::current_path().string();
            std::cout << "\nSaving binarization results to: " << output_dir << std::endl;
            
            // Compare ALL binarization methods
            std::cout << "Comparing ALL binarization methods..." << std::endl;
            
            // 1. Grayscale
            PPM grayPicture = GrayScale(picture);
            cv::Mat grayResult = PPMtoMat(grayPicture, height, width);
            cv::namedWindow("1. Grayscale", cv::WINDOW_NORMAL);
            cv::imshow("1. Grayscale", grayResult);
            std::string file1 = output_dir + "/01_Grayscale.jpg";
            if (cv::imwrite(file1, grayResult)) {
                std::cout << "✓ Saved: 01_Grayscale.jpg" << std::endl;
            } else {
                std::cerr << "✗ Failed to save: 01_Grayscale.jpg" << std::endl;
            }
            
            // 2. User threshold (default 127)
            int defaultThreshold = 127;
            PPM thresholdPicture = userThreshold(picture, defaultThreshold);
            cv::Mat thresholdResult = PPMtoMat(thresholdPicture, height, width);
            cv::namedWindow("2. User Threshold (127)", cv::WINDOW_NORMAL);
            cv::imshow("2. User Threshold (127)", thresholdResult);
            std::string file2 = output_dir + "/02_UserThreshold_T127.jpg";
            if (cv::imwrite(file2, thresholdResult)) {
                std::cout << "✓ Saved: 02_UserThreshold_T127.jpg" << std::endl;
            } else {
                std::cerr << "✗ Failed to save: 02_UserThreshold_T127.jpg" << std::endl;
            }
            
            // 3. Percentage black (default 50%)
            int defaultPercentage = 50;
            PPM percentagePicture = PercentageBlack(picture, defaultPercentage);
            cv::Mat percentageResult = PPMtoMat(percentagePicture, height, width);
            cv::namedWindow("3. Percentage Black (50%)", cv::WINDOW_NORMAL);
            cv::imshow("3. Percentage Black (50%)", percentageResult);
            std::string file3 = output_dir + "/03_PercentageBlack_50percent.jpg";
            if (cv::imwrite(file3, percentageResult)) {
                std::cout << "✓ Saved: 03_PercentageBlack_50percent.jpg" << std::endl;
            } else {
                std::cerr << "✗ Failed to save: 03_PercentageBlack_50percent.jpg" << std::endl;
            }
            
            // 4. Mean Iterative
            PPM meanIterativePicture = MeanIterative(picture);
            cv::Mat meanIterativeResult = PPMtoMat(meanIterativePicture, height, width);
            cv::namedWindow("4. Mean Iterative", cv::WINDOW_NORMAL);
            cv::imshow("4. Mean Iterative", meanIterativeResult);
            std::string file4 = output_dir + "/04_MeanIterative.jpg";
            if (cv::imwrite(file4, meanIterativeResult)) {
                std::cout << "✓ Saved: 04_MeanIterative.jpg" << std::endl;
            } else {
                std::cerr << "✗ Failed to save: 04_MeanIterative.jpg" << std::endl;
            }
            
            // 5. Entropy Selection
            PPM entropyPicture = EntropySelection(picture);
            cv::Mat entropyResult = PPMtoMat(entropyPicture, height, width);
            cv::namedWindow("5. Entropy Selection", cv::WINDOW_NORMAL);
            cv::imshow("5. Entropy Selection", entropyResult);
            std::string file5 = output_dir + "/05_EntropySelection.jpg";
            if (cv::imwrite(file5, entropyResult)) {
                std::cout << "✓ Saved: 05_EntropySelection.jpg" << std::endl;
            } else {
                std::cerr << "✗ Failed to save: 05_EntropySelection.jpg" << std::endl;
            }
            
            // 6. Minimum Error
            PPM minErrorPicture = MinimumError(picture);
            cv::Mat minErrorResult = PPMtoMat(minErrorPicture, height, width);
            cv::namedWindow("6. Minimum Error", cv::WINDOW_NORMAL);
            cv::imshow("6. Minimum Error", minErrorResult);
            std::string file6 = output_dir + "/06_MinimumError.jpg";
            if (cv::imwrite(file6, minErrorResult)) {
                std::cout << "✓ Saved: 06_MinimumError.jpg" << std::endl;
            } else {
                std::cerr << "✗ Failed to save: 06_MinimumError.jpg" << std::endl;
            }
            
            // 7. Fuzzy Minimum
            PPM fuzzyPicture = FuzyyMinimum(picture);
            cv::Mat fuzzyResult = PPMtoMat(fuzzyPicture, height, width);
            cv::namedWindow("7. Fuzzy Minimum", cv::WINDOW_NORMAL);
            cv::imshow("7. Fuzzy Minimum", fuzzyResult);
            std::string file7 = output_dir + "/07_FuzzyMinimum.jpg";
            if (cv::imwrite(file7, fuzzyResult)) {
                std::cout << "✓ Saved: 07_FuzzyMinimum.jpg" << std::endl;
            } else {
                std::cerr << "✗ Failed to save: 07_FuzzyMinimum.jpg" << std::endl;
            }
            
            std::cout << "\nAll binarization methods displayed and saved!" << std::endl;
            
            cv::waitKey(1);
            break;
        }

        case 10:
        {
            // Close all result windows (except original)
            std::vector<std::string> resultsToClose = {
                "Histogram Stretch Result",
                "Histogram Equalization Result",
                "Grayscale Result",
                "Binary Result (Threshold)",
                "Binary Result (Percentage)",
                "1. Grayscale",
                "2. User Threshold (127)",
                "3. Percentage Black (50%)",
                "4. Mean Iterative",
                "5. Entropy Selection",
                "6. Minimum Error",
                "7. Fuzzy Minimum"
            };
            
            for (const auto& window : resultsToClose)
            {
                cv::destroyWindow(window);
            }
            
            std::cout << "All result windows closed." << std::endl;
            break;
        }

        case 11:
        {
            running = false;
            std::cout << "Exiting program..." << std::endl;
            break;
        }

        default:
        {
            std::cout << "Invalid choice! Try again." << std::endl;
            break;
        }
        }
    }

    cv::destroyAllWindows();
    return 0;
}
