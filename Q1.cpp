#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <sys/time.h>
#include "x86intrin.h"



using namespace cv;
using namespace std;
using namespace std::chrono; 

int main( )
{

    cout << "Arman Rostami 810196609" << endl;
    cout << "Mohammad Saber Ebrahim Nejad 810196400" << endl << endl;

    struct timeval serial_start, serial_stop, parallel_start, parallel_stop;

	cv::Mat in_img_1 = cv::imread("CA03__Q1__Image__01.png", IMREAD_GRAYSCALE);
	cv::Mat in_img_2 = cv::imread("CA03__Q1__Image__02.png", IMREAD_GRAYSCALE);

	unsigned int NCOLS = in_img_1.cols;
	unsigned int NROWS = in_img_1.rows;

	cv::Mat out_img_1  (NROWS, NCOLS, CV_8U);
	cv::Mat out_img_2 (NROWS, NCOLS, CV_8U);

	unsigned char *in_image_1, *in_image_2, *out_image;

	in_image_1 = (unsigned char *) in_img_1.data;
    in_image_2 = (unsigned char *) in_img_2.data;
	out_image = (unsigned char *) out_img_1.data;

    gettimeofday(&serial_start, NULL); 
	for (int row = 0; row < NROWS; row++)
		for (int col = 0; col < NCOLS; col++)
			*(out_image + row * NCOLS + col) = abs(*(in_image_1 + row * NCOLS + col) - *(in_image_2 + row * NCOLS + col));
    gettimeofday(&serial_stop, NULL);


	__m128i *image_1_src, *image_2_src, *pRes;
	__m128i m1, m2, m3;

	image_1_src = (__m128i *) in_img_1.data;
    image_2_src = (__m128i *) in_img_2.data;
	pRes = (__m128i *) out_img_2.data;

    gettimeofday(&parallel_start, NULL);
    for (int i = 0; i < NROWS; i++)
		for (int j = 0; j < NCOLS / 16; j++)
		{
			m1 = _mm_loadu_si128(image_1_src + i * NCOLS/16 + j) ;
			m2 = _mm_loadu_si128(image_2_src + i * NCOLS/16 + j) ;
			m3 = _mm_sub_epi8 (m1, m2);
            m3 = _mm_abs_epi8(m3);
			_mm_storeu_si128 (pRes + i * NCOLS/16 + j, m3);
		}
    gettimeofday(&parallel_stop, NULL);


    long serial_micros = serial_stop.tv_usec - serial_start.tv_usec;
    long parallel_micros = parallel_stop.tv_usec - parallel_start.tv_usec;
    float speedup = (float)serial_micros / parallel_micros;

    cout << "Parallel: " << serial_micros << "us" << endl;
    cout << "Parallel: " << parallel_micros << "us" << endl;
    cout << "Speedup: " << speedup << endl;


	cv::namedWindow( "serial output", cv::WINDOW_AUTOSIZE );
	cv::imshow( "serial output", out_img_1 );
	cv::namedWindow( "parallel output", cv::WINDOW_AUTOSIZE );
	cv::imshow( "parallel output", out_img_2 );

	waitKey(0);

    return 0;
}
