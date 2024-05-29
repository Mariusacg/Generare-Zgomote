// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <cmath>
#include <cstdlib> 
#include <iostream> 
#include <time.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

/* Histogram display function - display a histogram using bars (simlilar to L3 / PI)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i<hist_cols; i++)
	if (hist[i] > max_hist)
		max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

Mat generateGaussianNoise(int rows, int cols, int mean, int variance)
{
	Mat noise = Mat::zeros(rows, cols, CV_8UC1);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j+=2)
        {	//Algoritmul Box-Muller
			constexpr double two_pi = 2.0 * M_PI;

    		double u1, u2;
    		do
    		{
    		    u1 = (double)rand()/ RAND_MAX;
    		}
    		while (u1 == 0);
    		u2 = (double)rand() / RAND_MAX;

    		double mag = (double)variance * sqrt(-2.0 * log(u1));
    		auto z0  = mag * cos(two_pi * u2) + mean;
    		auto z1  = mag * sin(two_pi * u2) + mean;

            noise.at<uchar>(i, j) = (z0 > 255.0) ? 255 : (z0 < 0.0) ? 0 : (uchar)z0;

			noise.at<uchar>(i, j + 1) = (z1 > 255.0) ? 255 : (z1 < 0.0) ? 0 : (uchar)z1;
        }
    return noise;
}

void addGaussianNoise()
{
	char fname[MAX_PATH];
    while(openFileDlg(fname))
    {
        Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat original = src.clone();
		int mean, variance;

		printf("Baga media :");
		scanf("%d", &mean);

		printf("\nBaga variatia :");
		scanf("%d", &variance);

        Mat noise = generateGaussianNoise(src.rows, src.cols, mean, variance); //generez matricea cu zgomote

		//int hist[256]{0};

		//for (int i = 0; i < src.rows; i++)
			//for (int j = 0; j < src.cols; j++)
				//hist[noise.at<uchar>(i, j)]++;

		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols; j++)
			{
				int sum = src.at<uchar>(i, j) + noise.at<uchar>(i, j) - mean; //Adaug la fiecare pixel zgomotul rezultat
				src.at<uchar>(i, j) = (sum > 255.0) ? 255 : (sum < 0.0) ? 0 : sum; // verific rezultatul sa fie in intervalul 0 - 255
			}
		//Show la histograma Imaginii care contine Noise-ul
		//showHistogram("Histograma", hist, 256, 256);

        imshow("Gaussian noise",src);
		imshow("Original image", original);
        waitKey();
    }
}

void addSaltPepperNoise()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat original = src.clone();

		double salt, pepper;
		
		printf("Introdu probabilitatea(0 - 100) ca un pixel sa devina pixel alb :");
		scanf("%lf", &salt);
		salt /= 100.0f;	//normalizare in intervalul 0.0f - 1.0f

		salt = 1.0 - salt;	//Inversez probabilitatea

		printf("\nIntrodu probabilitatea(0 - 100) ca un pixel sa devina pixel negru :");
		scanf("%lf", &pepper);
		pepper /= 100.0f; //normalizare in intervalul 0.0f - 1.0f

		printf("\n");

		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols; j++)
			{
				double sansa = (double)rand() / RAND_MAX;	//Creez un nou numar random intre 0.0f si 1.0f
				// Testez daca numarul generat este mai mare decat probabilitatea pentru salt sau mai mic decat probabilitatea pentru pepper
				src.at<uchar>(i, j) = (sansa > salt) ? 255 : (sansa < pepper) ? 0 : src.at<uchar>(i, j);
			}

		imshow("Salt&pepper noise", src);
		imshow("Original image", original);
		waitKey();
	}
}

void addSpeckleNoise()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat original = src.clone();

		double noiserange;

		printf("Introdu magnitudinea zgomotului(0 - 100) :");
		scanf("%lf", &noiserange);
		noiserange /= 100; //normalizare in intervalul 0.0f - 1.0f

		printf("\n");

		int rows = src.rows;
		int cols = src.cols;

		Mat specklenoise = Mat(rows, cols, CV_64F);

		for(int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				//Creez matricea cu zgomote care este in intervalul 0 - magnitudine citita
				specklenoise.at<double>(i, j) = ((double)rand() / RAND_MAX) * noiserange; 
																						
		Mat doublesrc;
		//Aplicam conversia Uchar- float pentru a putea inmulti matricea cu cea de zgomot.
		src.convertTo(doublesrc, CV_64F);

		doublesrc = doublesrc.mul(specklenoise);
		
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
			{
				int sum;
				if ((double)rand()/ RAND_MAX > 0.5f) //Lasam la voia sorti daca adaugam zgomotul rezultat sau il scadem (50% - 50%)
					sum = src.at<uchar>(i, j) + (uchar)doublesrc.at<double>(i, j); 
				else
					sum = src.at<uchar>(i, j) - (uchar)doublesrc.at<double>(i, j); 
				src.at<uchar>(i, j) = (sum > 255.0) ? 255 : (sum < 0.0) ? 0 : sum; // verific rezultatul sa fie in intervalul 0 - 255
			}

		imshow("Speckle noise", src);
		imshow("Original image", original);
		waitKey();
	}
}

int main()
{
	int op;
	srand(time(0));
	do
	{
		system("cls");
		destroyAllWindows();
		printf(" 1 - Generate Gaussian noise\n");
		printf(" 2 - Generate Salt&Pepper noise\n");
		printf(" 3 - Generate Speckle noise\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		switch (op)
		{
			case 1:
				addGaussianNoise();
				break;
			case 2:
				addSaltPepperNoise();
				break;
			case 3:
				addSpeckleNoise();
				break;
			default:
				break;
		}
	}
	while (op!=0);
	return 0;
}