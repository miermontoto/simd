#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <time.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include <immintrin.h>  // std::intrinsic functions

using namespace std;
using namespace cimg_library;

#define REPETITIONS 17
#define ITEMSPERPACKET (sizeof(__m256) / sizeof(float))

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
    int nPixels = width * height;

    // Time variables necessary to display the execution time when the algorithm is done.
	struct timespec tStart, tEnd;
	double dElapsedTimeS;

    // Information about the source image is stored for later use.
    width  = srcImage.width();
	height = srcImage.height();
	nComp  = srcImage.spectrum();

    // Resulting image allocation
    pDstImage = (float *) malloc (nPixels * nComp * sizeof(float));
	if (pDstImage == NULL) {
		perror("Couldn't allocate resulting image.");
		exit(1);
	}

    // Number of packets necessary to process an image.
    int nPackets = (nPixels / ITEMSPERPACKET);
    // If it isn't divisible, add one more packet (it won't be completely processed)
    if ( ((nPixels * sizeof(float)) % sizeof(__m256)) != 0) nPackets++;

    // Memory allocation for the destination image's components.
    float *pRdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    float *pGdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    float *pBdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    
    // 256bit packets are created in order to store those components in memory.
    __m256 vR, vG, vB; 


    // !!! no estoy del todo seguro de que esto sea necesario, se debería trabajar por
    // !!! paquetes DENTRO del propio algoritmo y cada paquete se genera dentro, por lo
    // !!! que igualar a -1 no tiene mucho sentido. aunque también podría estar perfecto,
    // !!! no tengo ni idea.

    // Every component is initialized as "-1".
    *(__m256 *) pRdest = _mm256_set1_ps(-1);
    *(__m256 *)(pRdest + ITEMSPERPACKET)     = _mm256_set1_ps(-1);
    *(__m256 *)(pRdest + ITEMSPERPACKET * 2) = _mm256_set1_ps(-1);
    *(__m256 *) pGdest = _mm256_set1_ps(-1);
    *(__m256 *)(pGdest + ITEMSPERPACKET)     = _mm256_set1_ps(-1);
    *(__m256 *)(pGdest + ITEMSPERPACKET * 2) = _mm256_set1_ps(-1);
    *(__m256 *) pBdest = _mm256_set1_ps(-1);
    *(__m256 *)(pBdest + ITEMSPERPACKET)     = _mm256_set1_ps(-1);
    *(__m256 *)(pBdest + ITEMSPERPACKET * 2) = _mm256_set1_ps(-1);
    
	// pointer initialization. same as in single-thread??

	// source image pointers
	pRsrc = srcImage.data();         // red component
	pGsrc = pRsrc + height * width;  // green component
	pBsrc = pGsrc + height * width;  // blue component

	// help image pointers
	pRaid = aidImage.data();         // red component
	pGaid = pRaid + height * width;  // green component
	pBaid = pGaid + height * width;  // blue component

	// destination image pointers
	pRdest = pDstImage;               // red component
	pGdest = pRdest + height * width; // green component
	pBdest = pGdest + height * width; // blue component

    for(int i = 0; i < REPETITIONS; i++) {

        // Packets for each image are initialized.
        __m256 kRsrc, kGsrc, kBsrc;
        __m256 kRaid, kGaid, kBaid;
        __m256 kRdest, kGdest, kBdest;

        // The algorithm should operate with EACH block of pixels, each one of them being
        // 'itemsPerPacket' in size.
        for(int k = 0; k < nPixels; k += ITEMSPERPACKET) {

            // aquí se lee un paquete
            // aquí se convierte un paquete de alguna manera a floats???
            // se hace lo que se hacía en single-thread para operar con los floats.
            // min max para evitar valores inváldos...
            // se guardan los resultados??? no sé cómo

            for(int j = 0; j < ITEMSPERPACKET; j++) {
                // para cada float, algo. no tengo ni idea.
            }

        }
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