#include "ImageBase.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char **argv)
{
	///////////////////////////////////////// Exemple d'un seuillage d'image
	char cNomImgLue[250];//, cNomImgEcrite[250], cNomImgFlou[250];
	//int S, colored,N;
    //stringstream nom_fichier_sortie;

    /*
	if (argc != 6)
	{
		printf("Usage: ImageIn.pgm ImageOut.pgm K colored\n");
		return 1;
	}*/

    if(argc != 3) {
        cout << "Usage : c/d Image.ppm ou pgm.\n c : Compression\nD : Decompression\n";
        return EXIT_FAILURE;
    }

	sscanf (argv[2],"%s",cNomImgLue);

    /*
    string image_lue = cNomImgLue;
    image_lue = image_lue.substr(0, image_lue.size()-4);
    */

	ImageBase imIn;
	imIn.load(cNomImgLue);

    /*
    nom_fichier_sortie << "Sortie/" << image_lue;
    string s = nom_fichier_sortie.str();
     */

    if(*argv[1] == 'c') {
        cout << "Compression" << endl;
        imIn.compressionDuGitan();
    } else {
        cout << "Decompression" << endl;
        imIn.DecompressionDuGitan(true).save("Sortie/Decompression.ppm");
    }

    /*
	if(colored) {
		imIn.to_YCbCr().save(const_cast<char *> (s.c_str()));
		imIn.PSNR(imIn.to_YCbCr());
		//imIn.to_YCbCr().save("Sortie/YCBCR");
		//imIn.to_YCbCr().K_mean(S,1).save("Sortie/YCBCR_KM");
		//imIn.to_YCbCr().K_mean(S,1).to_RGB().save("Sortie/YCBCR_KM_RGB");
		imIn.PSNR(imIn.to_YCbCr().K_mean(S, 1).to_RGB());
		imIn.transform_vaguellette(N, 0, 0, imIn.getWidth());
		//imIn.save(cNomImgEcrite);
	}
	else{
		vector<string> dico;
		unsigned char * valeurs = imIn.getData();
		int height = imIn.getHeight(), width = imIn.getWidth();
		vector < unsigned long> ids;
		imIn.dictionnaire(valeurs,height*width,dico, ids);
		printf("creation du dictionnaire done\n");
		imIn.from_Dico(height,width,ids, dico).save("Sortie/from_dico.pgm");
		printf("Creation de l'image a partir du dico: done\n");
	}
    */

	printf("done\n");
	return 0;
}
