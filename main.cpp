#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <time.h>
#include <fstream>
#include <immintrin.h>  // std::intrinsic functions

using namespace std;
using namespace cimg_library;

#define REPETITIONS 17
#define ITEMSPERPACKET (sizeof(__m256) / sizeof(float))

const char* SOURCE_IMG      = "bailarina.bmp"; // source image's file name.
const char* HELP_IMG        = "background_V.bmp"; // aid image's file name.
const char* DESTINATION_IMG = "result.bmp"; // resulting image's file name.


int main() {
    
    // Check to see if both files to process actually exist.
    std::ifstream file1(SOURCE_IMG);
    std::ifstream file2(HELP_IMG);

	if(!file1 || !file2) {
		printf("Couldn't locate source and help images.\n");
		exit(1);
	}

    CImg<float> srcImage(SOURCE_IMG); // Source image's information
	CImg<float> aidImage(HELP_IMG);   // Aid image's information

    float *pRsrc, *pGsrc, *pBsrc; // Source image pointers
	float *pRaid, *pGaid, *pBaid; // Aid image pointers
    float *pRdest, *pGdest, *pBdest; // Resulting image pointers
    float *pDstImage;
    uint width, height; // General image information variables
	uint nComp;
    int nPixels;

    // Time variables necessary to display the execution time when the algorithm is done.
	struct timespec tStart, tEnd;
	double dElapsedTimeS;

    // Information about the source image is stored for later use.
    width  = srcImage.width();
	height = srcImage.height();
	nComp  = srcImage.spectrum();

    nPixels = width * height;

    // Resulting image allocation
    pDstImage = (float *) malloc (nPixels * nComp * sizeof(float));
	if (pDstImage == NULL) {
		perror("Couldn't allocate resulting image.");
		exit(1);
	}

    // The width and height of both images are checked to be equal.
	if(srcImage.width() != aidImage.width() || srcImage.height() != aidImage.height()) {
		perror("Images to blend don't have the same size.");
		exit(1);
	}

    // Number of packets necessary to process an image.
    int nPackets = (nPixels / ITEMSPERPACKET);

    // If it isn't divisible, add one more packet (it won't be completely processed)
    if ( ((nPixels * sizeof(float)) % sizeof(__m256)) != 0) nPackets++;
    
	// pointer initialization. same as in single-thread

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

    // Starting time
	if (clock_gettime(CLOCK_REALTIME, &tStart) == -1) {
		printf("Error: couldn't obtain starting time print.");
		exit(1);
	}

    for(int i = 0; i < REPETITIONS; i++) {

        // The algorithm should operate with EACH block of pixels, each one of them being
        // 'itemsPerPacket' in size.
        for(int k = 0; k < nPixels; k += ITEMSPERPACKET) {

            // Packets for each image are initialized.
            __m256 kRsrc, kGsrc, kBsrc; 
            __m256 kRaid, kGaid, kBaid;
            __m256 kRdest, kGdest, kBdest;

            // Packets preloaded with desired values to avoid suboptimal repetition.
            __m256 p255 = _mm256_set1_ps(255), p256 = _mm256_set1_ps(256);
            __m256 p0 = _mm256_set1_ps(0), p1 = _mm256_set1_ps(1);

            // Packets are read and translated from float*
            kRsrc = _mm256_loadu_ps(pRsrc);
            kGsrc = _mm256_loadu_ps(pGsrc);
            kBsrc = _mm256_loadu_ps(pBsrc);

            kRaid = _mm256_loadu_ps(pRaid);
            kGaid = _mm256_loadu_ps(pGaid);
            kBaid = _mm256_loadu_ps(pBaid);

            // The algorithm itself using SIMD instructions only.
            
            // (255 - pRaid[i]))
            kRdest = _mm256_sub_ps(p255, kRaid);
            kGdest = _mm256_sub_ps(p255, kGaid);
            kBdest = _mm256_sub_ps(p255, kBaid);

            // (256 * (255 - pRaid[i])
            kRdest = _mm256_mul_ps(p256, kRdest);
            kGdest = _mm256_mul_ps(p256, kGdest);
            kBdest = _mm256_mul_ps(p256, kBdest);

            // (256 * (255 - pRaid[i])) / (pRsrc[i] + 1)
            kRdest = _mm256_div_ps(kRdest, _mm256_add_ps(kRsrc, p1));
            kGdest = _mm256_div_ps(kGdest, _mm256_add_ps(kGsrc, p1));
            kBdest = _mm256_div_ps(kBdest, _mm256_add_ps(kBsrc, p1));

            // 255 - ((256 * (255 - pRaid[i])) / (pRsrc[i] + 1))
            kRdest = _mm256_sub_ps(p255, kRdest);
            kGdest = _mm256_sub_ps(p255, kGdest);
            kBdest = _mm256_sub_ps(p255, kBdest);

            
            // Trim offscale values (<0, >255)
            kRdest = _mm256_max_ps(p0, _mm256_min_ps(p255, kRdest));
            kGdest = _mm256_max_ps(p0, _mm256_min_ps(p255, kGdest));
            kBdest = _mm256_max_ps(p0, _mm256_min_ps(p255, kBdest));
            

            // Float pointers to final packets. These allow to select each pixel.
            float *prd = (float *) &kRdest;
            float *pgd = (float *) &kGdest;
            float *pbd = (float *) &kBdest;

            // Convert packets into floats.
            for(long unsigned int j = 0; j < ITEMSPERPACKET; j++) {
                *pRdest = *prd;
                *pGdest = *pgd;
                *pBdest = *pbd;
                prd++ ; pgd++ ; pbd++ ;
                
                pRdest++ ; pBdest++ ; pGdest++ ;
            }

            pRsrc += ITEMSPERPACKET ; pGsrc += ITEMSPERPACKET ; pBsrc += ITEMSPERPACKET ;
            pRaid += ITEMSPERPACKET ; pGaid += ITEMSPERPACKET ; pBaid += ITEMSPERPACKET ;
        }

        // Each time the algorithm is repeated, go back to the starting pixel.
        //printf("algorithm finished %d times.\n", i + 1);
        pRdest -= nPixels ; pGdest -= nPixels ; pBdest -= nPixels ;
        pRsrc -= nPixels ; pGsrc -= nPixels ; pBsrc -= nPixels ;
        pRaid -= nPixels ; pGaid -= nPixels ; pBaid -= nPixels ;
    }

    if (clock_gettime(CLOCK_REALTIME, &tEnd) == -1) {
		printf("Error: couldn't obtain final time print.");
		exit(1);
	}

    // Print final execution time
	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf ("Final execution time: %f\n", dElapsedTimeS);

	CImg<float> dstImage(pDstImage, width, height, 1, nComp);

    //dstImage.cut(0, 255);
	dstImage.save(DESTINATION_IMG);   // the image is saved to file.
    if(dstImage.mean() == 0.0) printf("Blank image.\n");
	dstImage.display(); // the resulting image is shown on screen.
	free(pDstImage); // the memory used by the pointers is freed.

    return 0;
}