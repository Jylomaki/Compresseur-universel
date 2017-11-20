#include "ImageBase.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char **argv)
{
	///////////////////////////////////////// Exemple d'un seuillage d'image
	char cNomImgLue[250], cNomImgEcrite[250], cNomImgFlou[250];
	int S, colored,N;
    stringstream nom_fichier_sortie;

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


    string image_lue = cNomImgLue;
    image_lue = image_lue.substr(0, image_lue.size()-4);


	ImageBase imIn, imOut;
	imIn.load(cNomImgLue);


    nom_fichier_sortie << "Sortie/" << image_lue;
    string s = nom_fichier_sortie.str();

    imIn.to_YCbCr().save(const_cast<char *> (s.c_str()));

    imIn.PSNR(imIn.to_YCbCr());
    //imIn.to_YCbCr().save("Sortie/YCBCR");
	//imIn.to_YCbCr().K_mean(S,1).save("Sortie/YCBCR_KM");
	//imIn.to_YCbCr().K_mean(S,1).to_RGB().save("Sortie/YCBCR_KM_RGB");
	imIn.PSNR(imIn.to_YCbCr().K_mean(S,1).to_RGB());
	imIn.transform_vaguellette(N,0,0,imIn.getWidth());
	//imIn.save(cNomImgEcrite);

	printf("done\n");
	return 0;
}
