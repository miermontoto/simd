#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <time.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include <immintrin.h>  // std::intrinsic functions

using namespace std;
using namespace cimg_library;

#define Repeats 17
#define Item_Packet (sizeof(__m256)/sizeof(float))

const char* SOURCE_IMG      = "bailarina.bmp"; // source image's file name.
const char* HELP_IMG        = "background_V.bmp"; // aid image's file name.
const char* DESTINATION_IMG = "result.bmp"; // resulting image's file name.


int main() {

    CImg<float> srcImage(SOURCE_IMG); // Source image's information
	CImg<float> aidImage(HELP_IMG);   // Aid image's information

    float *pRsrc, *pGsrc, *pBsrc; // Source image pointers
	float *pRaid, *pGaid, *pBaid; // Aid image pointers
    float *pDstImage;
    uint width, height; // general image information
	uint nComp;

    // Time variables are initialized.
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

    //Calcula el numero de paquetes que necesitan para el tamaño de las imagenes
    int nPackets = ((width*height) * sizeof(float)/sizeof(__m256));
    //Si no es exacto se le añade un paquete más que no se complete
    if ( (((width*height) * sizeof(float))%sizeof(__m256)) != 0) {
        nPackets++;
	}

    //Reserva de memoria para todos los componentes de la imagen final
    float *pRdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    float *pGdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    float *pBdest = (float *)_mm_malloc(sizeof(__m256) * nPackets, sizeof(__m256));
    
    //Paquetes de 256bits para guardar los resultados en memoria
    __m256 vR, vG, vB; 

    //Inicialización de los elementos de destino al valor -1
    *(__m256 *) pRdest = _mm256_set1_ps(-1);
    *(__m256 *)(pRdest + Item_Packet)     = _mm256_set1_ps(-1);
    *(__m256 *)(pRdest + Item_Packet * 2) = _mm256_set1_ps(-1);
    *(__m256 *) pGdest = _mm256_set1_ps(-1);
    *(__m256 *)(pGdest + Item_Packet)     = _mm256_set1_ps(-1);
    *(__m256 *)(pGdest + Item_Packet * 2) = _mm256_set1_ps(-1);
    *(__m256 *) pBdest = _mm256_set1_ps(-1);
    *(__m256 *)(pBdest + Item_Packet)     = _mm256_set1_ps(-1);
    *(__m256 *)(pBdest + Item_Packet * 2) = _mm256_set1_ps(-1);

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

    if (clock_gettime(CLOCK_REALTIME, &tStart)==-1) {
		printf("Couldn't obtain final time print.");
		exit(1);
	}
    





}