#include "ImageBase.h"
#include <stdio.h>



int main(int argc, char **argv)
{
	///////////////////////////////////////// Exemple d'un seuillage d'image
	char cNomImgLue[250], cNomImgEcrite[250], cNomImgFlou[250];
	int S, colored,N;

	if (argc != 6)
	{
		printf("Usage: ImageIn.pgm ImageOut.pgm K colored\n");
		return 1;
	}
	sscanf (argv[1],"%s",cNomImgLue) ;
	sscanf (argv[2],"%s",cNomImgEcrite);
	sscanf (argv[3],"%d",&S);
	sscanf (argv[4],"%d",&colored);
	sscanf (argv[5],"%d",&N);



	
	ImageBase imIn, imOut;
	imIn.load(cNomImgLue);
	imIn.to_YCrCb().save(cNomImgEcrite);
	imIn.PSNR(imIn.to_YCrCb());
	imIn.transform_vaguellette(N,0,0,imIn.getWidth());
	//imIn.save(cNomImgEcrite);

	printf("done\n");
	return 0;
}
