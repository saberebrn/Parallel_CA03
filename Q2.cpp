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
    const float ALPHA = 0.5;

    cout << "Arman Rostami 810196609" << endl;
    cout << "Mohammad Saber Ebrahim Nejad 810196400" << endl << endl;

    struct timeval serial_start, serial_stop, parallel_start, parallel_stop;

	cv::Mat in_img_1 = cv::imread("CA03__Q2__Image__01.png", IMREAD_GRAYSCALE);
	cv::Mat in_img_2 = cv::imread("CA03__Q2__Image__02.png", IMREAD_GRAYSCALE);

	unsigned int IMG_1_NCOLS = in_img_1.cols;
	unsigned int IMG_1_NROWS = in_img_1.rows;
    unsigned int IMG_2_NCOLS = in_img_2.cols;
	unsigned int IMG_2_NROWS = in_img_2.rows;

	cv::Mat out_img_1 = cv::imread("CA03__Q2__Image__01.png", IMREAD_GRAYSCALE);
	cv::Mat out_img_2 = cv::imread("CA03__Q2__Image__01.png", IMREAD_GRAYSCALE);

	unsigned char *in_image_1, *in_image_2, *out_image;

	in_image_1 = (unsigned char *) in_img_1.data;
    in_image_2 = (unsigned char *) in_img_2.data;
	out_image = (unsigned char *) out_img_1.data;

    gettimeofday(&serial_start, NULL); 
	for (int row = 0; row < IMG_2_NROWS; row++)
		for (int col = 0; col < IMG_2_NCOLS; col++){
            int result = *(in_image_1 + row * IMG_1_NCOLS + col) + (*(in_image_2 + row * IMG_2_NCOLS + col) * ALPHA);
			*(out_image + row * IMG_1_NCOLS + col) = result > 255 ? 255 : result;
        }
    gettimeofday(&serial_stop, NULL);


	__m128i *image_1_src, *image_2_src, *pRes;
	__m128i m1, m2, m3;

	image_1_src = (__m128i *) in_img_1.data;
    image_2_src = (__m128i *) in_img_2.data;
	pRes = (__m128i *) out_img_2.data;

    gettimeofday(&parallel_start, NULL);
    for (int i = 0; i < IMG_2_NROWS; i++)
		for (int j = 0; j < IMG_2_NCOLS / 16; j++)
		{
			m1 = _mm_loadu_si128(image_1_src + i * IMG_1_NCOLS/16 + j);
			m2 = _mm_loadu_si128(image_2_src + i * IMG_2_NCOLS/16 + j);
			m2 = _mm_srli_epi16(m2, 1);
            m2 = _mm_and_si128(m2, _mm_set1_epi8(0x7f));
            m3 = _mm_adds_epu8(m1, m2);
			_mm_storeu_si128 (pRes + i * IMG_1_NCOLS/16 + j, m3);
		}
    gettimeofday(&parallel_stop, NULL);


    long serial_micros = serial_stop.tv_usec - serial_start.tv_usec;
    long parallel_micros = parallel_stop.tv_usec - parallel_start.tv_usec;
    float speedup = (float)serial_micros / parallel_micros;

    cout << "Serial: " << serial_micros << "us" << endl;
    cout << "Parallel: " << parallel_micros << "us" << endl;
    cout << "Speedup: " << speedup << endl;


	cv::namedWindow( "serial output", cv::WINDOW_AUTOSIZE );
	cv::imshow( "serial output", out_img_1 );
	cv::namedWindow( "parallel output", cv::WINDOW_AUTOSIZE );
	cv::imshow( "parallel output", out_img_2 );

	waitKey(0);

    return 0;
}
