#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <time.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include <immintrin.h>  // std::intrinsic functions

using namespace std;
using namespace cimg_library;

#define repetitions 17
#define itemsPerPacket (sizeof(__m256)/sizeof(float))

const char* SOURCE_IMG      = "bailarina.bmp"; // source image's file name.
const char* HELP_IMG        = "background_V.bmp"; // aid image's file name.
const char* DESTINATION_IMG = "result.bmp"; // resulting image's file name.


int main() {

    CImg<float> srcImage(SOURCE_IMG); // Source image's information
	CImg<float> aidImage(HELP_IMG);   // Aid image's information

    float *pRsrc, *pGsrc, *pBsrc; // Source image pointers
	float *pRaid, *pGaid, *pBaid; // Aid image pointers
    float *pDstImage;
    uint width, height; // General image information variables
	uint nComp;

    // Time variables necessary to display the execution time when the algorithm is done.
	struct timespec tStart, tEnd;
	double dElapsedTimeS;

    // Information about the source image is stored for later use.
    width  = srcImage.width();
	height = srcImage.height();
	nComp  = srcImage.spectrum();

    // Resulting image allocation
    pDstImage = (float *) malloc (width * height * nComp * sizeof(float));
	if (pDstImage == NULL) {
		perror("Couldn't allocate resulting image.");
		exit(1);
	}

    // Number of packets necessary to process an image.
    int nPackets = ((width * height) / itemsPerPacket);
    // If it isn't divisible, add one more packet (it won't be completely processed)
    if ( ((width * height * sizeof(float)) % sizeof(__m256)) != 0) nPackets++;

    // Memory allocation for the destination image's components.
    float *pRdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    float *pGdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    float *pBdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    
    // 256bit packets are created in order to store those components in memory.
    __m256 vR, vG, vB; 

    // Every component is initialized as "-1".
    *(__m256 *) pRdest = _mm256_set1_ps(-1);
    *(__m256 *)(pRdest + itemsPerPacket)     = _mm256_set1_ps(-1);
    *(__m256 *)(pRdest + itemsPerPacket * 2) = _mm256_set1_ps(-1);
    *(__m256 *) pGdest = _mm256_set1_ps(-1);
    *(__m256 *)(pGdest + itemsPerPacket)     = _mm256_set1_ps(-1);
    *(__m256 *)(pGdest + itemsPerPacket * 2) = _mm256_set1_ps(-1);
    *(__m256 *) pBdest = _mm256_set1_ps(-1);
    *(__m256 *)(pBdest + itemsPerPacket)     = _mm256_set1_ps(-1);
    *(__m256 *)(pBdest + itemsPerPacket * 2) = _mm256_set1_ps(-1);

    pRsrc = srcImage.data();        // componente roja
	pGsrc = pRsrc + height * width; // componente verde
	pBsrc = pGsrc + height * width; // componente azul

	// Punteros a la imagen de apoyo
	pRaid = aidImage.data();        // componente roja
	pGaid = pRaid + height * width; // componente verde
	pBaid = pGaid + height * width; // componente azul

	// Punteros a la imagen resultante
	pRdest = pDstImage;               // componente roja
	pGdest = pRdest + height * width; // componente verde
	pBdest = pGdest + height * width; // componente azul

    for(int i = 0; i < repetitions; i++) {
        // el algoritmo traducido va aquí <-
    }

    if (clock_gettime(CLOCK_REALTIME, &tStart)==-1) {
		printf("Couldn't obtain final time print.");
		exit(1);
	}

    // Print final execution time
	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;

	printf ("Final execution time: %f\n", dElapsedTimeS);

	CImg<float> dstImage(pDstImage, width, height, 1, nComp);

	dstImage.save(DESTINATION_IMG);   // the image is saved to file.
	dstImage.display(); // the resulting image is shown on screen.
	free(pDstImage);    // the memory used by the pointers is freed.

    return 0;
}