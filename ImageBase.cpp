/******************************************************************************
* ICAR_Library
*
* Fichier : ImageBase.cpp
*
* Description : Voir le fichier .h
*
* Auteur : Mickael Pinto
*
* Mail : mickael.pinto@live.fr
*
* Date : Octobre 2012
*
*******************************************************************************/

#include "ImageBase.h"
#include "image_ppm.h"
#include <cstring>
#include <math.h>
#include <vector>
#include <string>


ImageBase::ImageBase(void)
{
	isValid = false;
	init();
}

ImageBase::ImageBase(int imWidth, int imHeight, bool isColor)
{
	isValid = false;
	init();

	color = isColor;
	height = imHeight;
	width = imWidth;
	nTaille = height * width * (color ? 3 : 1);

	if(nTaille == 0)
		return;

	allocation_tableau(data, OCTET, nTaille);
	dataD = (double*)malloc(sizeof(double) * nTaille);
	isValid = true;
}


ImageBase::~ImageBase(void)
{
	reset();
}

void ImageBase::init()
{
	if(isValid)
	{
		free(data);
		free(dataD);
	}

	data = 0;
	dataD = 0;
	height = width = nTaille = 0;
	isValid = false;
}

void ImageBase::reset()
{
	if(isValid)
	{
		free(data);
		free(dataD);
	}
	isValid = false;
}

void ImageBase::load(char *filename)
{
	init();

	int l = strlen(filename);

	if(l <= 4) // Le fichier ne peut pas etre que ".pgm" ou ".ppm"
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut �tre que '.pgm' ou '.ppm'");
		exit(0);
	}

	int nbPixel = 0;

	if( strcmp(filename + l - 3, "pgm") == 0) // L'image est en niveau de gris
	{
		color = false;
		lire_nb_lignes_colonnes_image_pgm(filename, &height, &width);
		nbPixel = height * width;

		nTaille = nbPixel;
		allocation_tableau(data, OCTET, nTaille);
		lire_image_pgm(filename, data, nbPixel);
	}
	else if( strcmp(filename + l - 3, "ppm") == 0) // L'image est en couleur
	{
		color = true;
		lire_nb_lignes_colonnes_image_ppm(filename, &height, &width);
		nbPixel = height * width;

		nTaille = nbPixel * 3;
		allocation_tableau(data, OCTET, nTaille);
		lire_image_ppm(filename, data, nbPixel);
	}
	else
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut �tre que .pgm ou .ppm");
		exit(0);
	}

	dataD = (double*)malloc(sizeof(double) * nTaille);

	isValid = true;
}

bool ImageBase::save(char *filename)
{
	if(!isValid)
	{
		printf("Sauvegarde de l'image impossible : L'image courante n'est pas valide\n");
		exit(0);
	}

	if(color)
		ecrire_image_ppm(filename, data,  height, width);
	else
		ecrire_image_pgm(filename, data,  height, width);

	return true;
}

ImageBase *ImageBase::getPlan(PLAN plan)
{
	if( !isValid || !color )
		return 0;

	ImageBase *greyIm = new ImageBase(width, height, false);

	switch(plan)
	{
	case PLAN_R:
		planR(greyIm->data, data, height * width);
		break;
	case PLAN_G:
		planV(greyIm->data, data, height * width);
		break;
	case PLAN_B:
		planB(greyIm->data, data, height * width);
		break;
	default:
		printf("Il n'y a que 3 plans, les valeurs possibles ne sont donc que 'PLAN_R', 'PLAN_G', et 'PLAN_B'");
		exit(0);
		break;
	}

	return greyIm;
}

void ImageBase::copy(const ImageBase &copy)
{
	reset();

	isValid = false;
	init();

	color = copy.color;
	height = copy.height;
	width = copy.width;
	nTaille = copy.nTaille;
	isValid = copy.isValid;

	if(nTaille == 0)
		return;

	allocation_tableau(data, OCTET, nTaille);
	dataD = (double*)malloc(sizeof(double) * nTaille);
	isValid = true;

	for(int i = 0; i < nTaille; ++i)
	{
		data[i] = copy.data[i];
		dataD[i] = copy.dataD[i];
	}

}

unsigned char *ImageBase::operator[](int l)
{
	if(!isValid)
	{
		printf("L'image courante n'est pas valide");
		exit(0);
	}

	if((!color && l >= height) || (color && l >= height*3))
	{
		printf("L'indice se trouve en dehors des limites de l'image");
		exit(0);
	}

	return data+l*width;
}


ImageBase ImageBase::threshold(int S){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	printf("%d, %d\n",this->getWidth(), this->getHeight());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if ((*this)[x][y] < S)
				imOut[x][y] = 0;
			else imOut[x][y] = 255;
		}

	return imOut;


}


ImageBase ImageBase::thresholdFond(int S){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if ((*this)[x][y] < S)
				imOut[x][y] = 255;
			else imOut[x][y] = 0;
		}

	return imOut;


}

ImageBase ImageBase::threshold4(int S1, int S2, int S3){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if ((*this)[x][y] < S1)
				imOut[x][y] = 0;
			else if((*this)[x][y] < S2)
				imOut[x][y] = 85;
			else if((*this)[x][y] < S3)
				imOut[x][y] = 170;
			else imOut[x][y] = 255;
		}

	return imOut;


}

ImageBase ImageBase::thresholdPPM(int SR, int SG, int SB){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if ((*this)[y*3][x*3+0] < SR)
				imOut[y*3][x*3+0] = 0;
			else
				imOut[y*3][x*3+0] = 255;

			if((*this)[y*3][x*3+1] < SG)
				imOut[y*3][x*3+1] = 0;
			else
				imOut[y*3][x*3+1] = 255;

			if((*this)[y*3][x*3+2] < SB)
				imOut[y*3][x*3+2] = 0;
			else
				imOut[y*3][x*3+2] = 255;
		}

	return imOut;


}


ImageBase ImageBase::thresholdPPMhist(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	int minR = 40;
	int maxR = 220;
	int minGB = 30;
	int maxGB = 170;

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if ((*this)[y*3][x*3+0] < minR){
				imOut[y*3][x*3+0] = static_cast<unsigned char>(minR);
			}else{
				imOut[y*3][x*3+0] = (*this)[y*3][x*3+0];
			}
			if((*this)[y*3][x*3+0] > maxR){
				imOut[y*3][x*3+0] = static_cast<unsigned char>(maxR);
			}else{
				imOut[y*3][x*3+0] = (*this)[y*3][x*3+0];
			}

			if((*this)[y*3][x*3+1] < minGB)
				imOut[y*3][x*3+1] = static_cast<unsigned char>(minGB);
			else
				imOut[y*3][x*3+1] =(*this)[y*3][x*3+1];

			if((*this)[y*3][x*3+1] > maxGB)
				imOut[y*3][x*3+1] = static_cast<unsigned char>(maxGB);
			else
				imOut[y*3][x*3+1] =(*this)[y*3][x*3+1];

			if((*this)[y*3][x*3+2] < minGB)
				imOut[y*3][x*3+2] = static_cast<unsigned char>(minGB);
			else
				imOut[y*3][x*3+2] = (*this)[y*3][x*3+2];

			if((*this)[y*3][x*3+2] > maxGB)
				imOut[y*3][x*3+2] = static_cast<unsigned char>(maxGB);
			else
				imOut[y*3][x*3+2] = (*this)[y*3][x*3+2];
		}

	return imOut;


}

void ImageBase::profilpgmLC(char *lc, int index){

	std::ofstream outputFile;
	outputFile.open("profil.dat");

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if(strcmp(lc, "l") == 0)
				if(x == index){
					printf("ligne : x =%d, y=%d val=%d \n", x, y,(*this)[x][y]);
					outputFile << std::to_string((*this)[x][y]) << "\n";
				}
			if(strcmp(lc, "c") == 0)
				if(y == index){
					printf("colonne : x =%d, y=%d val=%d \n", x, y,(*this)[x][y]);
					outputFile << std::to_string((*this)[x][y]) << "\n";
				}
		}

	outputFile.close();

}

void ImageBase::histPGM(){


	int hist[255];

	for(int i=0; i<255; i++){
		hist[i]=0;
	}

	std::ofstream outputFile;
	outputFile.open("hist.dat");

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			hist[(*this)[x][y]]++;
		}


	for(int i=0; i<255; i++){
		outputFile << i << " " << std::to_string(hist[i]) << "\n";
	}

	outputFile.close();


}


ImageBase ImageBase::expansionDynamique(){

	int hist[255];
	int expHist[255];
	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int i=0; i<255; i++){
		hist[i]=0;
		expHist[i]=0;
	}

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			hist[(*this)[x][y]]++;
		}

	int Vmin =0;
	for(int i=0; i<255; i++){
		if(hist[i]<1){
			Vmin =i;
			break;
		}
	}
	int Vmax =255;
	for(int i=255; i>0; i--){
		if(hist[i]<1){
			Vmax =i;
			break;
		}
	}

	int beta = 255 / (Vmax-Vmin);
	int alpha = ((-255) * Vmin) / (Vmax-Vmin);

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			imOut[x][y] = static_cast<unsigned char>((255/Vmax-Vmin)*(-(Vmin)+(*this)[x][y]));
			printf("%d, %d\n ",(*this)[x][y], imOut[x][y]);
		}

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			expHist[imOut[x][y]]++;
		}

	std::ofstream outputFile;
	outputFile.open("expHist.dat");
	for(int i=0; i<255; i++){
		outputFile << i << " " << std::to_string(expHist[i]) << "\n";
	}
	outputFile.close();
	printf("%d\n %d\n",alpha, beta);
	return imOut;


}

ImageBase ImageBase::expansionDynamiquePPM(){

	int histR[255];
	int histG[255];
	int histB[255];
	int expHistR[255];
	int expHistG[255];
	int expHistB[255];

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int i=0; i<255; i++){
		histR[i]=0;
		histG[i]=0;
		histB[i]=0;
	}

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			histR[(*this)[y*3][x*3+0]]++;
			histG[(*this)[y*3][x*3+1]]++;
			histB[(*this)[y*3][x*3+2]]++;
		}

	int VminR =0;
	for(int i=0; i<255; i++){
		if(histR[i]<1){
			VminR =i;
			break;
		}
	}
	int VmaxR =255;
	for(int i=255; i>0; i--){
		if(histR[i]<1){
			VmaxR =i;
			break;
		}
	}

	int betaR = 255 / (VmaxR-VminR);
	int alphaR = ((-255) * VminR) / (VmaxR-VminR);


	int VminG =0;
	for(int i=0; i<255; i++){
		if(histG[i]<1){
			VminG =i;
			break;
		}
	}
	int VmaxG =255;
	for(int i=255; i>0; i--){
		if(histG[i]<1){
			VmaxG =i;
			break;
		}
	}

	int betaG = 255 / (VmaxG-VminG);
	int alphaG = ((-255) * VminG) / (VmaxG-VminG);


	int VminB =0;
	for(int i=0; i<255; i++){
		if(histB[i]<1){
			VminB =i;
			break;
		}
	}
	int VmaxB =255;
	for(int i=255; i>0; i--){
		if(histB[i]<1){
			VmaxB =i;
			break;
		}
	}

	int betaB = 255 / (VmaxB-VminB);
	int alphaB = ((-255) * VminB) / (VmaxB-VminB);

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			imOut[y*3][x*3+0] = (255/double(VmaxR-VminR))*(-(VminR)+(*this)[y*3][x*3+0]);
			imOut[y*3][x*3+1] = (255/double(VmaxG-VminG))*(-(VminG)+(*this)[y*3][x*3+1]);
			imOut[y*3][x*3+2] = (255/double(VmaxB-VminB))*(-(VminB)+(*this)[y*3][x*3+2]);
		}

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			expHistR[imOut[y*3][x*3+0]]++;
			expHistG[imOut[y*3][x*3+1]]++;
			expHistB[imOut[y*3][x*3+1]]++;

		}

	std::ofstream outputFile;

	outputFile.open("expHist.dat");
	for(int i=0; i<255; i++){
		outputFile << i << " " << std::to_string(expHistR[i]) << " " << std::to_string(expHistG[i]) << " " << std::to_string(expHistB[i])<< "\n";

	}
	outputFile.close();
	printf("%d\n%d\n%d\n%d\n%d\n%d\n%d\n",betaR, betaR, betaG, betaG, betaG, betaB, betaB);
	return imOut;

}

void ImageBase::histPPM(){

	int histR[255];
	int histG[255];
	int histB[255];

	for(int i=0; i<255; i++){
		histR[i]=0;
		histG[i]=0;
		histB[i]=0;
	}

	std::ofstream outputFile;
	outputFile.open("histPPM.dat");

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			histR[(*this)[y*3][x*3+0]]++;
			histG[(*this)[y*3][x*3+1]]++;
			histB[(*this)[y*3][x*3+2]]++;
		}


	for(int i=0; i<255; i++){
		outputFile << i << " " << std::to_string(histR[i]) << " " << std::to_string(histG[i]) << " " << std::to_string(histB[i]) << "\n";
	}

	outputFile.close();

}

int* ImageBase::histo(){

	int* histR = new int[255];

	for(int i=0; i<255; i++){
		histR[i]=0;
	}

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			histR[(*this)[y*3][x*3+0]]++;
		}

	// for(int i=0; i<255;i++){
	// 	printf("%d \n", histR[i]);
	// }

	return histR;
}

int* ImageBase::compareTab(const int* tab1, const int* tab2){

	int nbDiff=0;
	int length = 255;
    auto * tabDiff = new int[length];

	for(int i=0; i<255; i++){
		tabDiff[i]=0;
	}

	for(int i=0;i<length;i++){
		if((tab1[i]-tab2[i])!=0)
			tabDiff[i]++;

	 }
	return tabDiff;

}

ImageBase ImageBase::erosion(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	printf("%d , %d\n", this->getHeight(), this->getWidth());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if((*this)[x][y] == 255){

				if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
					//printf("%d , %d\n", x, y);
					imOut[x-1][y] = imOut[x+1][y] = imOut[x][y-1] = imOut[x][y+1] = 255;
				}
			}
		}

	return imOut;

}



ImageBase ImageBase::erosionPPM(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			int max = 0;
			if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
				if((*this)[y*3][x*3+0] > max){
					max = (*this)[y*3][x*3+0];
				}
				if((*this)[y*3][(x-1)*3+0] > max){
					max = (*this)[y*3][(x-1)*3+0];
				}
				if((*this)[y*3][(x+1)*3+0] > max){
					max = (*this)[y*3][(x+1)*3+0];
				}
				if((*this)[(y-1)*3][x*3+0] > max){
					max = (*this)[(y-1)*3][x*3+0];
				}
				if((*this)[(y+1)*3][x*3+0] > max){
					max = (*this)[(y+1)*3][x*3+0];
				}
				imOut[y*3][(x-1)*3+0] = imOut[y*3][(x+1)*3+0] = imOut[(y-1)*3][x*3+0] = imOut[(y+1)*3][x*3+0]= static_cast<unsigned char>(max);

				max = 0;
				if((*this)[y*3][x*3+1] > max){
					max = (*this)[y*3][x*3+1];
				}
				if((*this)[y*3][(x-1)*3+1] > max){
					max = (*this)[y*3][(x-1)*3+1];
				}
				if((*this)[y*3][(x+1)*3+1] > max){
					max = (*this)[y*3][(x+1)*3+1];
				}
				if((*this)[(y-1)*3][x*3+1] > max){
					max = (*this)[(y-1)*3][x*3+1];
				}
				if((*this)[(y+1)*3][x*3+1] > max){
					max = (*this)[(y+1)*3][x*3+1];
				}
				imOut[y*3][(x-1)*3+1] = imOut[y*3][(x+1)*3+1] = imOut[(y-1)*3][x*3+1] = imOut[(y+1)*3][x*3+1]= static_cast<unsigned char>(max);

				max = 0;
				if((*this)[y*3][x*3+2] > max){
					max = (*this)[y*3][x*3+2];
				}
				if((*this)[y*3][(x-1)*3+2] > max){
					max = (*this)[y*3][(x-1)*3+2];
				}
				if((*this)[y*3][(x+1)*3+2] > max){
					max = (*this)[y*3][(x+1)*3+2];
				}
				if((*this)[(y-1)*3][x*3+2] > max){
					max = (*this)[(y-1)*3][x*3+2];
				}
				if((*this)[(y+1)*3][x*3+2] > max){
					max = (*this)[(y+1)*3][x*3+2];
				}
				imOut[y*3][(x-1)*3+2] = imOut[y*3][(x+1)*3+2] = imOut[(y-1)*3][x*3+2] = imOut[(y+1)*3][x*3+2]= static_cast<unsigned char>(max);

			}
		}

	return imOut;

}


ImageBase ImageBase::erosionPGM(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			int max = 0;
			if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
				if((*this)[x][y] > max){
					max = (*this)[x][y];
				}
				if((*this)[x-1][y] > max){
					max = (*this)[x-1][y];
				}
				if((*this)[x+1][y] > max){
					max = (*this)[x+1][y];
				}
				if((*this)[x][y-1] > max){
					max = (*this)[x][y-1];
				}
				if((*this)[x][y+1] > max){
					max = (*this)[x][y+1];
				}
				imOut[x-1][y] = imOut[x+1][y] = imOut[x][y-1] = imOut[x][y+1] = static_cast<unsigned char>(max);
			}
		}

	return imOut;

}

ImageBase ImageBase::dilatation(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if((*this)[x][y] == 0){

				if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
					//printf("%d , %d\n", x, y);
					imOut[x-1][y] = imOut[x+1][y] = imOut[x][y-1] = imOut[x][y+1] = 0;
				}
			}else{
				imOut[x][y] = 255;
			}
		}

	return imOut;

}


ImageBase ImageBase::dilatationPPM(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			int min = 255;
			if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
				//printf("%d , %d\n", x, y);
				if((*this)[y*3][x*3+0] < min){
					min = (*this)[y*3][x*3+0];
				}
				if((*this)[y*3][(x-1)*3+0] < min){
					min = (*this)[y*3][(x-1)*3+0];
				}
				if((*this)[y*3][(x+1)*3+0] < min){
					min = (*this)[y*3][(x+1)*3+0];
				}
				if((*this)[(y-1)*3][x*3+0] < min){
					min = (*this)[(y-1)*3][x*3+0];
				}
				if((*this)[(y+1)*3][x*3+0] < min){
					min = (*this)[(y+1)*3][x*3+0];
				}
				imOut[y*3][(x-1)*3+0] = imOut[y*3][(x+1)*3+0] = imOut[(y-1)*3][x*3+0] = imOut[(y+1)*3][x*3+0]= static_cast<unsigned char>(min);

				min = 255;
				if((*this)[y*3][x*3+1] < min){
					min = (*this)[y*3][x*3+1];
				}
				if((*this)[y*3][(x-1)*3+1] < min){
					min = (*this)[y*3][(x-1)*3+1];
				}
				if((*this)[y*3][(x+1)*3+1] < min){
					min = (*this)[y*3][(x+1)*3+1];
				}
				if((*this)[(y-1)*3][x*3+1] < min){
					min = (*this)[(y-1)*3][x*3+1];
				}
				if((*this)[(y+1)*3][x*3+1] < min){
					min = (*this)[(y+1)*3][x*3+1];
				}
				imOut[y*3][(x-1)*3+1] = imOut[y*3][(x+1)*3+1] = imOut[(y-1)*3][x*3+1] = imOut[(y+1)*3][x*3+1]= static_cast<unsigned char>(min);

				min = 255;
				if((*this)[y*3][x*3+2] < min){
					min = (*this)[y*3][x*3+2];
				}
				if((*this)[y*3][(x-1)*3+2] < min){
					min = (*this)[y*3][(x-1)*3+2];
				}
				if((*this)[y*3][(x+1)*3+2] < min){
					min = (*this)[y*3][(x+1)*3+2];
				}
				if((*this)[(y-1)*3][x*3+2] < min){
					min = (*this)[(y-1)*3][x*3+2];
				}
				if((*this)[(y+1)*3][x*3+2] < min){
					min = (*this)[(y+1)*3][x*3+2];
				}
				imOut[y*3][(x-1)*3+2] = imOut[y*3][(x+1)*3+2] = imOut[(y-1)*3][x*3+2] = imOut[(y+1)*3][x*3+2] = static_cast<unsigned char>(min);
			}
		}

	return imOut;

}

ImageBase ImageBase::dilatationPGM(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			int min = 255;
			if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
				if((*this)[x][y] < min){
					min = (*this)[x][y];
				}
				if((*this)[x-1][y] < min){
					min = (*this)[x-1][y];
				}
				if((*this)[x+1][y] < min){
					min = (*this)[x+1][y];
				}
				if((*this)[x][y-1] < min){
					min = (*this)[x][y-1];
				}
				if((*this)[x][y+1] < min){
					min = (*this)[x][y+1];
				}
				imOut[x-1][y] = imOut[x+1][y] = imOut[x][y-1] = imOut[x][y+1] = static_cast<unsigned char>(min);
			}
		}

	return imOut;

}


ImageBase ImageBase::fermeture(){

	return this->dilatation().erosion();

}

ImageBase ImageBase::fermeturePPM(){

	return this->dilatationPPM().erosionPPM();

}

ImageBase ImageBase::fermeturePGM(){

	return this->dilatationPGM().erosionPGM();

}

ImageBase ImageBase::ouverture(){

	return this->erosion().dilatation();

}

ImageBase ImageBase::ouverturePPM(){

	return this->erosionPPM().dilatationPPM();

}

ImageBase ImageBase::ouverturePGM(){

	return this->erosionPGM().dilatationPGM();

}

ImageBase ImageBase::segmentation(ImageBase seuillee, ImageBase dilatee){


	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			imOut[x][y] = 255;
			if(seuillee[x][y] != dilatee[x][y]){
				imOut[x][y] = 0;
			}else {
				imOut[x][y] = 255;
			}
		}

	return imOut;


}


ImageBase ImageBase::greyLevel(){

	ImageBase imOut(this->getWidth(), this->getHeight(), false);

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			imOut[y][x] = static_cast<unsigned char>((*this)[y*3][x*3+0]*0.2126 + (*this)[y*3][x*3+1]*0.7152 + (*this)[y*3][x*3+2]*0.0722);
		}

	printf("%d, %d \n",imOut.getWidth(), imOut.getHeight());
	return imOut;

}

ImageBase ImageBase::flouCouleur(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
				int testR = ((*this)[(y-1)*3][(x-1)*3+0] + (*this)[y*3][(x-1)*3+0] + (*this)[(y+1)*3][(x-1)*3+0] + (*this)[(y-1)*3][x*3+0] + (*this)[y*3][x*3+0] + (*this)[(y+1)*3][x*3+0] + (*this)[(y-1)*3][(x+1)*3+0] + (*this)[y*3][(x+1)*3+0] + (*this)[(y+1)*3][(x+1)*3+0])/9;
				int V = ((*this)[(y-1)*3][(x-1)*3+1] + (*this)[y*3][(x-1)*3+1] + (*this)[(y+1)*3][(x-1)*3+1] + (*this)[(y-1)*3][x*3+1] + (*this)[y*3][x*3+1] + (*this)[(y+1)*3][x*3+1] + (*this)[(y-1)*3][(x+1)*3+1] + (*this)[y*3][(x+1)*3+1] + (*this)[(y+1)*3][(x+1)*3+1])/9;
				int B = ((*this)[(y-1)*3][(x-1)*3+2] + (*this)[y*3][(x-1)*3+2] + (*this)[(y+1)*3][(x-1)*3+2] + (*this)[(y-1)*3][x*3+2] + (*this)[y*3][x*3+2] + (*this)[(y+1)*3][x*3+2] + (*this)[(y-1)*3][(x+1)*3+2] + (*this)[y*3][(x+1)*3+2] + (*this)[(y+1)*3][(x+1)*3+2])/9;
				printf("%d, %d, %d\n",testR, V, B);

				imOut[y*3][x*3+0] = static_cast<unsigned char>(testR);
				imOut[y*3][x*3+1] = static_cast<unsigned char>(V);
				imOut[y*3][x*3+2] = static_cast<unsigned char>(B);
			}

		}

	return imOut;


}

ImageBase ImageBase::flouFond(ImageBase fond){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	for(int x = 0; x < this->getHeight(); ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if((x != 0 )&&( y != 0 )&&( x != this->getHeight()-1 )&&( y != this->getWidth()-1)){
				if(fond[y][x] == 255){
					int testR = ((*this)[(y-1)*3][(x-1)*3+0] + (*this)[y*3][(x-1)*3+0] + (*this)[(y+1)*3][(x-1)*3+0] + (*this)[(y-1)*3][x*3+0] + (*this)[y*3][x*3+0] + (*this)[(y+1)*3][x*3+0] + (*this)[(y-1)*3][(x+1)*3+0] + (*this)[y*3][(x+1)*3+0] + (*this)[(y+1)*3][(x+1)*3+0])/9;
					int V = ((*this)[(y-1)*3][(x-1)*3+1] + (*this)[y*3][(x-1)*3+1] + (*this)[(y+1)*3][(x-1)*3+1] + (*this)[(y-1)*3][x*3+1] + (*this)[y*3][x*3+1] + (*this)[(y+1)*3][x*3+1] + (*this)[(y-1)*3][(x+1)*3+1] + (*this)[y*3][(x+1)*3+1] + (*this)[(y+1)*3][(x+1)*3+1])/9;
					int B = ((*this)[(y-1)*3][(x-1)*3+2] + (*this)[y*3][(x-1)*3+2] + (*this)[(y+1)*3][(x-1)*3+2] + (*this)[(y-1)*3][x*3+2] + (*this)[y*3][x*3+2] + (*this)[(y+1)*3][x*3+2] + (*this)[(y-1)*3][(x+1)*3+2] + (*this)[y*3][(x+1)*3+2] + (*this)[(y+1)*3][(x+1)*3+2])/9;
					//printf("%d, %d, %d\n",testR, V, B);

					imOut[y*3][x*3+0] = static_cast<unsigned char>(testR);
					imOut[y*3][x*3+1] = static_cast<unsigned char>(V);
					imOut[y*3][x*3+2] = static_cast<unsigned char>(B);

				}else{
					imOut[y*3][x*3+0] = (*this)[y*3][x*3+0];
					imOut[y*3][x*3+1] = (*this)[y*3][x*3+1];
					imOut[y*3][x*3+2] = (*this)[y*3][x*3+2];
				}

			//printf("fond: %d, %d\n", y, x);

			}

		}

	return imOut;


}


ImageBase ImageBase::moyenne4Region(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	int moyenne_hautgauche =0;
	int moyenne_hautdroit =0;
	int moyenne_basgauche =0;
	int moyenne_basdroit =0;
	int variance_hautgauche =0;
	int variance_hautdroit =0;
	int variance_basgauche =0;
	int variance_basdroit =0;
	int width =  this->getWidth();
	int height = this->getHeight();
	int nbpix = this->getHeight()/2*width/2;


	for(int x = 0; x < height; ++x)
		for(int y = 0; y < width; ++y)
		{
			if(x<height/2 && y<width/2){
				moyenne_hautgauche += (*this)[x][y];
			}
			if(x<height/2 && y>=width/2 && y<width){
				moyenne_hautdroit += (*this)[x][y];
			}
			if(x>=height/2 && y<width/2){
				moyenne_basgauche += (*this)[x][y];
			}
			if(x>=height/2 && y>=width/2 && y<width){
				moyenne_basdroit += (*this)[x][y];
			}


		}

	moyenne_hautgauche = moyenne_hautgauche/(nbpix);
	moyenne_hautdroit = moyenne_hautdroit/(nbpix);
	moyenne_basgauche = moyenne_basgauche/(nbpix);
	moyenne_basdroit = moyenne_basdroit/(nbpix);

	for(int x = 0; x < height; ++x)
		for(int y = 0; y < this->getWidth(); ++y)
		{
			if(x<height/2 && y<width/2){
				imOut[x][y] = static_cast<unsigned char>(moyenne_hautgauche);
				variance_hautgauche += ((*this)[x][y]-moyenne_hautgauche)*((*this)[x][y]-moyenne_hautgauche);
			}
			if(x<height/2 && y>=width/2 && y<width){
				imOut[x][y] = static_cast<unsigned char>(moyenne_hautdroit);
				variance_hautdroit += ((*this)[x][y]-moyenne_hautdroit)*((*this)[x][y]-moyenne_hautdroit);
			}
			if(x >=height/2 && y<width/2){
				imOut[x][y] = static_cast<unsigned char>(moyenne_basgauche);
				variance_basgauche += ((*this)[x][y]-moyenne_basgauche)*((*this)[x][y]-moyenne_basgauche);
			}
			if(x >=height/2 && y>=width/2 && y<width){
				imOut[x][y] = static_cast<unsigned char>(moyenne_basdroit);
				variance_basdroit += ((*this)[x][y]-moyenne_basdroit)*((*this)[x][y]-moyenne_basdroit);
			}

		}

	printf("%d, %d\n",nbpix, variance_hautgauche);
	variance_hautgauche = variance_hautgauche/nbpix;
	variance_hautdroit = variance_hautdroit/nbpix;
	variance_basgauche = variance_basgauche/nbpix;
	variance_basdroit = variance_basdroit/nbpix;
	printf("%d, %d, %d, %d\n",moyenne_hautgauche, moyenne_hautdroit, moyenne_basgauche, moyenne_basdroit);
	printf("%f, %f, %f, %f\n",sqrt(variance_hautgauche), sqrt(variance_hautdroit), sqrt(variance_basgauche), sqrt(variance_basdroit));

	return imOut;
}

void ImageBase::recursRegion(ImageBase &imOut, int idepart, int jdepart, int taille, int S){

	if(taille>10){
		int moyenne_hautgauche =0;
		int moyenne_hautdroit =0;
		int moyenne_basgauche =0;
		int moyenne_basdroit =0;
		int variance_hautgauche =0;
		int variance_hautdroit =0;
		int variance_basgauche =0;
		int variance_basdroit =0;
		int cote = taille / 2;
		int nbpix = cote*cote;
		printf("%d\n", taille);

		for(int x = idepart; x < idepart+taille; ++x)
			for(int y = jdepart; y <jdepart+taille; ++y)
			{

				if(x<idepart+cote && y<jdepart+cote){
					moyenne_hautgauche += (*this)[x][y];
					variance_hautgauche += (*this)[x][y] * (*this)[x][y];
				}
				if(x<idepart+cote && y>=jdepart+cote){
					moyenne_hautdroit += (*this)[x][y];
					variance_hautdroit += (*this)[x][y] * (*this)[x][y];
				}
				if(x>=idepart+cote && y<jdepart+cote){
					moyenne_basgauche += (*this)[x][y];
					variance_basgauche += (*this)[x][y] * (*this)[x][y];
				}
				if(x>=idepart+cote && y>=jdepart+cote){
					moyenne_basdroit += (*this)[x][y];
					variance_basdroit += (*this)[x][y] * (*this)[x][y];
				}

			}


		moyenne_hautgauche = moyenne_hautgauche/(nbpix);
		moyenne_hautdroit = moyenne_hautdroit/(nbpix);
		moyenne_basgauche = moyenne_basgauche/(nbpix);
		moyenne_basdroit = moyenne_basdroit/(nbpix);

		variance_hautgauche = variance_hautgauche/nbpix - moyenne_hautgauche*moyenne_hautgauche;
		variance_hautdroit = variance_hautdroit/nbpix - moyenne_hautdroit*moyenne_hautdroit;
		variance_basgauche = variance_basgauche/nbpix - moyenne_basgauche*moyenne_basgauche;
		variance_basdroit = variance_basdroit/nbpix - moyenne_basdroit*moyenne_basdroit;

		auto et_hg = (int)sqrt(variance_hautgauche);
		auto et_hd = (int)sqrt(variance_hautdroit);
		auto et_bg = (int)sqrt(variance_basgauche);
		auto et_bd = (int)sqrt(variance_basdroit);

		for(int x = idepart; x < idepart+taille; ++x)
			for(int y = jdepart; y <jdepart+taille; ++y)
			{

				if(x<idepart+cote && y<jdepart+cote){
					imOut[x][y] = static_cast<unsigned char>(moyenne_hautgauche);
				}
				if(x<idepart+cote && y>=jdepart+cote){
					imOut[x][y] = static_cast<unsigned char>(moyenne_hautdroit);
				}
				if(x>=idepart+cote && y<jdepart+cote){
					imOut[x][y] = static_cast<unsigned char>(moyenne_basgauche);
				}
				if(x>=idepart+cote && y>=jdepart+cote){
					imOut[x][y] = static_cast<unsigned char>(moyenne_basdroit);
				}

			}

		if(et_hg > S){
			recursRegion(imOut, idepart, jdepart, cote, S);
		}

		if(et_hd > S){
			recursRegion(imOut, idepart+cote, jdepart, cote, S);
		}

		if(et_bg > S){
			recursRegion(imOut, idepart, jdepart+cote, cote, S);
		}

		if(et_bd > S){
			recursRegion(imOut, idepart+cote, jdepart+cote, cote, S);
		}
	}

}


ImageBase ImageBase::getImg(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	if(this->getColor()){
		for(int x = 0; x < height; ++x)
			for(int y = 0; y < width; ++y)
			{
				imOut[y*3][x*3+0] = (*this)[y*3][x*3+0];
				imOut[y*3][x*3+1] = (*this)[y*3][x*3+1];
				imOut[y*3][x*3+2] = (*this)[y*3][x*3+2];
			}
	}

	else{
		for(int x = 0; x < height; ++x)
			for(int y = 0; y < width; ++y)
			{
				imOut[x][y] = (*this)[x][y];

			}

	}

	return imOut;
}

double ImageBase::dist(int c1[3], int c2[3]){

	return sqrt(pow(double(c1[0]-c2[0]),2) + pow(double(c1[1] - c2[1]),2) + pow(double(c1[2] - c2[2]),2));

}

ImageBase ImageBase::deux_mean(){

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	int C1[3];
	int C2[3];
	int currentPixel[3];

	double distC1, distC2;

	C1[0] = (*this)[0][0];
	C1[1] = (*this)[0][1];
	C1[2] = (*this)[0][2];

	C2[0] = (*this)[0][(height-1)*3];
	C2[1] = (*this)[0][(height-1)*3+1];
	C2[2] = (*this)[0][(height-1)*3+2];

	int r1_past,g1_past,b1_past;

	int r2_past,g2_past,b2_past;

	int r1 =0;
	int r2 = 0;
	int g1 = 0;
	int g2 = 0;
	int b1 = 0;
	int b2 = 0;
	int c1_count =0;
	int c2_count =0;

	do{


		r1 =0;
		r2 = 0;
		g1 = 0;
		g2 = 0;
		b1 = 0;
		b2 = 0;
		c1_count =0;
		c2_count =0;

		for(int x = 0; x < this->getHeight(); ++x)
			for(int y = 0; y < this->getWidth(); ++y)
			{

				currentPixel[0] = (*this)[y*3][x*3+0];
				currentPixel[1] = (*this)[y*3][x*3+1];
				currentPixel[2] = (*this)[y*3][x*3+2];

				distC1 = dist(currentPixel, C1);
				distC2 = dist(currentPixel, C2);

				if( distC1 < distC2){
					r1 += currentPixel[0];
					g1 += currentPixel[1];
					b1 += currentPixel[2];
					c1_count ++;

					imOut[y*3][x*3+0] = static_cast<unsigned char>(C1[0]);
					imOut[y*3][x*3+1] = static_cast<unsigned char>(C1[1]);
					imOut[y*3][x*3+2] = static_cast<unsigned char>(C1[2]);
				}
				else{
					r2 += currentPixel[0];
					g2 += currentPixel[1];
					b2 += currentPixel[2];
					c2_count ++;

					imOut[y*3][x*3+0] = static_cast<unsigned char>(C2[0]);
					imOut[y*3][x*3+1] = static_cast<unsigned char>(C2[1]);
					imOut[y*3][x*3+2] = static_cast<unsigned char>(C2[2]);
				}

			}
		r1 = r1 / c1_count;
		g1 /= c1_count;
		b1 /= c1_count;

		r2 = r2 / c2_count;
		g2 /= c2_count;
		b2 /= c2_count;

		r1_past = C1[0];
		g1_past = C1[1];
		b1_past = C1[2];

		r2_past = C2[0];
		g2_past = C2[1];
		b2_past = C2[2];

		C1[0] = r1;
		C1[1] = g1;
		C1[2] = b1;

		C2[0] = r2;
		C2[1] = g2;
		C2[2] = b2;


	}while(not(r1_past==r1 && g1_past == g1 && b1_past==b1 && r2_past==r2 && g2_past == g2 && b2_past==b2 ));

	return imOut;

}

ImageBase ImageBase::K_mean(int k, bool colored){
	//debug
	int nb_boucle=0;

	//palette
	int palette_R[k];
	int palette_G[k];
	int palette_B[k];

	//accumulateurs
	int palette_R_sum[k] = {0};
	int palette_G_sum[k] = {0};
	int palette_B_sum[k] = {0};
	int palette_count[k] = {0};

	//usage interne a boucle
	double minDist, currentDist;
	int currentPalette[3];
	int currentPixel[3];
	int old_color[3];
	int minID, currentID=0;

	//paramètres
	int sqrt_k = sqrt(k);
	int width =  this->getWidth();
	int height = this->getHeight();

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	ImageBase paletteOut(sqrt_k,sqrt_k,this->getColor());
	//printf("INIT de la palette\n");
	//On initialise la palette avec une grille de points
	for(int i =0; i<sqrt_k; i++){
		for(int j=0; j<sqrt_k; j++){
			//printf("INIT de la palette, currentID: %i\n",currentID);
			palette_R[currentID] = (*this)[i*3* height/sqrt_k][j*3* width/sqrt_k];
			palette_G[currentID] = (*this)[i*3* height/sqrt_k][j*3* width/sqrt_k+1];
			palette_B[currentID] = (*this)[i*3* height/sqrt_k][j*3* width/sqrt_k+2];
			currentID++;
		}
	}
	
	//printf("Begin iteration vers convergence\n");
	bool convergence = false;
	while(!convergence){
		convergence = true;
		//raccorder chaque point a son plus proche
		for(int i=0; i< height; i++){
			for(int j=0; j< width; j++){
				minDist = 257;
				minID = 0;
				currentPixel [0] = (*this)[i*3][j*3];
				currentPixel [1] = (*this)[i*3][j*3+1];
				currentPixel [2] = (*this)[i*3][j*3+2];
				
				//On cherche la couleur de la palette la plus proche du pixel courant
				for(int k_i=0; k_i < k; k_i++){
					currentPalette[0]= palette_R[k_i];
					currentPalette[1]= palette_G[k_i];
					currentPalette[2]= palette_B[k_i];

					currentDist = dist(currentPalette, currentPixel);
					if(minDist > currentDist){
						minDist = currentDist;
						minID = k_i;
					
					}
				}
				palette_R_sum[minID] += currentPixel[0];
				palette_G_sum[minID] += currentPixel[1];
				palette_B_sum[minID] += currentPixel[2];
				palette_count[minID] ++;
			}
		}
		//calculer la nouvelle couleur moyenne de chaque ensembles de la palette
		for(int i=0; i<k; i++){
			//On ne considère que les palettes qui ont des points
			if(palette_count[i] != 0){
				old_color[0]= palette_R[i];
				old_color[1]= palette_G[i];
				old_color[2]= palette_B[i];

				//Moyenne de la couleur des points approximés à la ieme couleur
				palette_R[i]= palette_R_sum[i]/palette_count[i];
				palette_G[i]= palette_G_sum[i]/palette_count[i];
				palette_B[i]= palette_B_sum[i]/palette_count[i];

				//reset de l'ensemble de point approximé à la ieme couleur
				palette_R_sum[i]=0;
				palette_G_sum[i]=0;
				palette_B_sum[i]=0;
				palette_count[i]=0;

				if(	old_color[0]!= palette_R[i] ||
					old_color[1] != palette_G[i] ||
					old_color[2] != palette_B[i]){
						convergence= false;
				}
			}
		}
		printf("Nb_boucle:%i\n",nb_boucle++);
	}

	// trier les couleurs de la palette par luminance:3,6,1
	// on veux trier la palette pour obtenir une image ndg lisible
	// On veux donc garder le n° dans la liste de luminance trier 
	// f(id) > rang_lumière (id)
	int palette_luminance[k];
	int palette_triee[k];
	int palette_triee_id[k];

	for(int i=0; i<k; i++){
		palette_luminance[i]= static_cast<unsigned char>(0.3 * palette_R[i] + 0.6* palette_G[i] + 0.1 * palette_B[i]);
	}
	// C'est pas un tri rapide, mais la j'ai la flemme
	
	for(int i=0; i<k; i++){
		int min = 257;
		minID = 0;
		for(int j=0; j<k; j++){
			if(palette_luminance[j] < min){
				min = palette_luminance[j];
				minID = j;
			}
		}
		palette_triee[minID]= i;
		palette_triee_id[i]=minID;
		palette_luminance[minID]=257;
	}

	// On met à la couleur correspondante a chaque points:
	for(int i=0; i< height; i++){
			for(int j=0; j< width; j++){
				minDist = 257;
				minID = 0;
				currentPixel [0] = (*this)[i*3][j*3];
				currentPixel [1] = (*this)[i*3][j*3+1];
				currentPixel [2] = (*this)[i*3][j*3+2];
				
				//On cherche la couleur de la palette la plus proche du pixel courant
				for(int k_i=0; k_i < k; k_i++){
					currentPalette[0]= palette_R[k_i];
					currentPalette[1]= palette_G[k_i];
					currentPalette[2]= palette_B[k_i];

					currentDist = dist(currentPalette, currentPixel);
					if(minDist > currentDist){
						minDist = currentDist;
						minID = k_i;
					}

					if(colored){//send color
						imOut[i*3][j*3+0] = static_cast<unsigned char>(palette_R[minID]);
						imOut[i*3][j*3+1] = static_cast<unsigned char>(palette_G[minID]);
						imOut[i*3][j*3+2] = static_cast<unsigned char>(palette_B[minID]);
					}
					else{//send ndg
						imOut[i*3][j*3+0] = static_cast<unsigned char>(palette_triee[minID]* 255/k);
						imOut[i*3][j*3+1] = static_cast<unsigned char>(palette_triee[minID]* 255/k);
						imOut[i*3][j*3+2] = static_cast<unsigned char>(palette_triee[minID]* 255/k);
					}
					

				}
				
			}
		}
	//dump de la palette
	currentID=0;
	for(int i=0; i<sqrt_k; i++){
		for(int j=0; j<sqrt_k; j++){
			paletteOut[i*3][j*3] = static_cast<unsigned char>(palette_R[palette_triee_id[currentID]]);
			paletteOut[i*3][j*3+1] = static_cast<unsigned char>(palette_G[palette_triee_id[currentID]]);
			paletteOut[i*3][j*3+2] = static_cast<unsigned char>(palette_B[palette_triee_id[currentID]]);
			currentID++;
		}
	}
	paletteOut.save((char*)"./Sortie/palette_dump.ppm");

	//calcul du PSNR
	int R_diffsum = 0, G_diffsum = 0,B_diffsum = 0;
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			R_diffsum += pow((*this)[i*3][j*3] - imOut[i*3][j*3],2);
			G_diffsum += pow((*this)[i*3][j*3+1] - imOut[i*3][j*3+1],2);
			B_diffsum += pow((*this)[i*3][j*3+2] - imOut[i*3][j*3+2],2);

		}
	}
	int EQM_color = (R_diffsum + G_diffsum + B_diffsum)/ (height*width);
	double PSNR = 10.0 * log10(pow(3*255.0,2.0)/(float)EQM_color);
	printf("K:%i\n",k);
	printf("PSNR:%lf \n",PSNR);
	return imOut;
}



ImageBase ImageBase::TP2_reechantillonage(int canal1, int canal2, int to_dump){
	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	int canal_vanilla=0;
	int x,y,x1, y1, weigth;
	int mean_count, col1_count, col2_count;

	if(canal1+canal2 == 1)
		canal_vanilla=2;
	else if(canal1+canal2 == 2)
		canal_vanilla=1;
	else
		canal_vanilla=0;


	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			imOut[i*3][j*3+canal_vanilla] = (*this)[i*3][j*3+canal_vanilla];

			//point vanilla
			if(i%2 == 0 && j%2 == 0){
				imOut[i*3][j*3+canal1] = (*this)[i*3][j*3+canal1];
				imOut[i*3][j*3+canal2] = (*this)[i*3][j*3+canal2];
			}
			else{
				mean_count=0;
				col1_count=0;
				col2_count=0;
				x=i;
				y=j;
				// On va récupérer les points
				//lignes:
				weigth=3;
				x1 = x;
				y1 = y+1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x-1;
				y1 = y;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}

				x1 = x;
				y1 = y-1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x+1;
				y1 = y;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}

				//diagonales:
				weigth=2;
				x1 = x+1;
				y1 = y-1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x-1;
				y1 = y-1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += 2;
				}
				x1 = x-1;
				y1 = y+1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x+1;
				y1 = y+1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += (*this)[x1*3][y1*3+canal1]*weigth;
					col2_count += (*this)[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				if(mean_count){
					imOut[i*3][j*3+canal1] = static_cast<unsigned char>(col1_count / mean_count);
					imOut[i*3][j*3+canal2] = static_cast<unsigned char>(col2_count / mean_count);
				}
				else{
					printf("A point had no valid neighbor\n");
				}
			}
		}
	}


	//Calcul du PSNR
	int R_diffsum = 0, G_diffsum = 0,B_diffsum = 0;
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			R_diffsum += pow((*this)[i*3][j*3] - imOut[i*3][j*3],2);
			G_diffsum += pow((*this)[i*3][j*3+1] - imOut[i*3][j*3+1],2);
			B_diffsum += pow((*this)[i*3][j*3+2] - imOut[i*3][j*3+2],2);

		}
	}
	
	int EQM_color = (R_diffsum + G_diffsum + B_diffsum)/ (height*width);
	double PSNR = 10.0 * log10(pow(3*255.0,2.0)/(float)EQM_color);
	printf("PSNR:%lf \n",PSNR);
	printf("canal_vanilla is :%d\n",canal_vanilla);
	return imOut;
}

ImageBase ImageBase::TP2_reechantillonage_YUV(int to_dump){
	ImageBase imTmp(this->getWidth(), this->getHeight(), this->getColor());
	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());

	int canal1=1, canal2=2, canal_vanilla=0;
	int x,y,x1, y1, weigth;
	int mean_count, col1_count, col2_count;
	int RGB[3], YCrCb[3];



	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			RGB[0]=(*this)[i*3][j*3+0];
			RGB[1]=(*this)[i*3][j*3+1];
			RGB[2]=(*this)[i*3][j*3+2];

			RGB_to_YCbCr(RGB, YCrCb);

			imTmp[i*3][j*3+0]=static_cast<unsigned char>(YCrCb[0]);
			imTmp[i*3][j*3+1]=static_cast<unsigned char>(YCrCb[1]);
			imTmp[i*3][j*3+2]=static_cast<unsigned char>(YCrCb[2]);
		}
	}

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			imOut[i*3][j*3+canal_vanilla] = imTmp[i*3][j*3+canal_vanilla];

			//point vanilla
			if(i%2 == 0 && j%2 == 0){
				imOut[i*3][j*3+canal1] = imTmp[i*3][j*3+canal1];
				imOut[i*3][j*3+canal2] = imTmp[i*3][j*3+canal2];
			}
			else{
				mean_count=0;
				col1_count=0;
				col2_count=0;
				x=i;
				y=j;
				// On va récupérer les points
				//lignes:
				weigth=3;
				x1 = x;
				y1 = y+1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x-1;
				y1 = y;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}

				x1 = x;
				y1 = y-1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x+1;
				y1 = y;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}

				//diagonales:
				weigth=2;
				x1 = x+1;
				y1 = y-1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x-1;
				y1 = y-1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += 2;
				}
				x1 = x-1;
				y1 = y+1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				x1 = x+1;
				y1 = y+1;
				if(validCoordinate(x1, y1,height,width)){
					col1_count += imTmp[x1*3][y1*3+canal1]*weigth;
					col2_count += imTmp[x1*3][y1*3+canal2]*weigth;
					mean_count += weigth;
				}
				if(mean_count){
					imOut[i*3][j*3+canal1] = static_cast<unsigned char>(col1_count / mean_count);
					imOut[i*3][j*3+canal2] = static_cast<unsigned char>(col2_count / mean_count);
				}
				else{
					printf("A point had no valid neighbor\n");
				}
			}
		}
	}

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			YCrCb[0]=imOut[i*3][j*3+0];
			YCrCb[1]=imOut[i*3][j*3+1];
			YCrCb[2]=imOut[i*3][j*3+2];

			YCbCr_to_RGB(YCrCb, RGB);

			imOut[i*3][j*3+0]=static_cast<unsigned char>(RGB[0]);
			imOut[i*3][j*3+1]=static_cast<unsigned char>(RGB[1]);
			imOut[i*3][j*3+2]=static_cast<unsigned char>(RGB[2]);
		}
	}

	//Calcul du PSNR
	int R_diffsum = 0, G_diffsum = 0,B_diffsum = 0;
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			R_diffsum += pow((*this)[i*3][j*3] - imOut[i*3][j*3],2);
			G_diffsum += pow((*this)[i*3][j*3+1] - imOut[i*3][j*3+1],2);
			B_diffsum += pow((*this)[i*3][j*3+2] - imOut[i*3][j*3+2],2);

		}
	}
	int EQM_color = (R_diffsum + G_diffsum + B_diffsum)/ (height*width);
	double PSNR = 10.0 * log10(pow(3*255.0,2.0)/(float)EQM_color);
	printf("PSNR:%lf \n",PSNR);
	printf("canal_vanilla is :%d\n",canal_vanilla);
	return imOut;
}


bool ImageBase::validCoordinate(int x, int y, int h, int w){
	return (0 <= x && x < h) && (0 <= y && y< w) && (x%2==0 && y%2 ==0);
}

void ImageBase::RGB_to_YCbCr(int *RGB, int *YCbCbr){
	int Y = static_cast<unsigned char>(RGB[0]*0.299 + RGB[1]*0.587 + RGB[2]*0.114);
	int Cb = static_cast<unsigned char>(-0.1687 * RGB[0] - 0.3313*RGB[1] + 0.5*RGB[2]+128);
	int Cr = static_cast<unsigned char>(0.5*RGB[0] - 0.4187*RGB[1] - 0.0813*RGB[2]+128);

	YCbCbr[0]=Y;
	YCbCbr[1]=Cb;
	YCbCbr[2]=Cr;
}

void ImageBase::YCbCr_to_RGB(int *YCbCbr, int *RGB){
	RGB[0] = static_cast<unsigned char>(YCbCbr[0]+ 1.402*(YCbCbr[2]-128));
	RGB[1] = static_cast<unsigned char>(YCbCbr[0]- 0.34414*(YCbCbr[1]-128) - 0.71414*(YCbCbr[2]-128));
	RGB[2] = static_cast<unsigned char>(YCbCbr[0]+ 1.772*(YCbCbr[1]-128));
}

void ImageBase::check_In_range(int* RGB){
	for(int i=0; i<3; i++)
	{
		if(RGB[i]>255){
			RGB[i]=255;
			printf("checked overflow");
		}
		else if (RGB[i]<0){
			RGB[i]=0;		
			printf("checked underflow");	
		}
	}
}

ImageBase ImageBase::to_YCbCr(){
	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	int YCrCb[3], RGB[3];
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			RGB[0]=(*this)[i*3][j*3+0];
			RGB[1]=(*this)[i*3][j*3+1];
			RGB[2]=(*this)[i*3][j*3+2];

			RGB_to_YCbCr(RGB, YCrCb);
			check_In_range(YCrCb);

			imOut[i*3][j*3+0]= static_cast<unsigned char>(YCrCb[0]);
			imOut[i*3][j*3+1]= static_cast<unsigned char>(YCrCb[1]);
			imOut[i*3][j*3+2]= static_cast<unsigned char>(YCrCb[2]);
		}
	}
	return imOut;
}

int ImageBase::to_range(int C){
	return (C+128)%256;
	if(C<0)
		return 0;
	else if(C>255)
		return 255;
	return C;
}

void ImageBase::transform_vaguellette(int n,int Hbegin, int Vbegin, int size){
	if(n <= 1){
		this->transform_vaguellette(Hbegin,Vbegin,size);
		printf(" recursion done on %d level\n",n);

	}
	else{
			//printf("### commencing recursion: macro\n");
			this->transform_vaguellette(Hbegin,Vbegin,size);
			//this->save("debug_im/0first_recur.ppm");
			//printf("~~ commencing recursion: micro\n");
			this->transform_vaguellette(n-1,Hbegin, Vbegin, size/2);
			//this->save("debug_im/1second recurP1.ppm");
			this->transform_vaguellette(n-1,Hbegin+size/2, Vbegin, size/2);
			//this->save("debug_im/2second recurP2.ppm");
			this->transform_vaguellette(n-1,Hbegin, Vbegin+size/2, size/2);
			//this->save("debug_im/3second recurP3.ppm");
			this->transform_vaguellette(n-1,Hbegin+size/2, Vbegin+size/2, size/2);
			//this->save("debug_im/4second recurP4.ppm");
			printf("~~ recursion done on level [%d]\n",n);

		}
}


void ImageBase::transform_vaguellette(int Hbegin, int Vbegin, int size){
	printf("call args: (H:%d,V:%d,size:%d)\n", Hbegin,Vbegin, size);
	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	// Prendre en 4 bloc: 	A: 1pix/4			B: dif horizontale
	//						C: dif verticale	D: dif diagonale 1 to 2
	int color, out;


	for(int c=0; c<3; c++){
		// block A
		//printf("A block\n");
		for(int i=0; i<size/2; i++){
			for(int j=0; j<size/2; j++){
				color=0;
				//do half the pixel on the whole pix
				int i_prime = Hbegin + i*2;
				int j_prime = Vbegin + j*2;
				for(int x=0; x<2; x++){
					for(int y=0; y<2; y++){
						color+= (*this)[(i_prime+x)*3][(j_prime+y)*3+c];
					}
				}
				//printf("(%d,%d):%d\n",i,j, color/4);
				int x = Hbegin +i;
				int y = Vbegin +j;
				imOut[x*3][y*3+c]= static_cast<unsigned char>((color/4));
			}
		}
		//printf("B block\n");
		for(int i=0; i<size/2; i++){
			for(int j=0; j<size/2; j++){
				int x= Hbegin + i*2;
				int y= Hbegin + j*2;

				int moyHH=(*this)[x*3][(y)*3+c]+ (*this)[(x)*3][(y+1)*3+c];
				moyHH = moyHH/2;
				int moyHB=(*this)[(x+1)*3][(y)*3+c]+ (*this)[(x+1)*3][(y+1)*3+c];
				moyHB = moyHB/2;
				//printf("(%d,%d)\n",(i*2+1),(j*2+1));

				x = Hbegin+size/2+i;
				y = Vbegin+j;
				//printf("(%d,%d):%d\n",x,y,to_range(moyHH - moyHB) );				
				imOut[x*3][y*3+c]= static_cast<unsigned char>(to_range(moyHH - moyHB));
			}
		}
		//printf("C block\n");
		for(int i=0; i<size/2; i++){
			for(int j=0; j<size/2; j++){
				int x= Hbegin + i*2;
				int y= Vbegin + j*2;

				int moyVG=(*this)[x*3][(y)*3+c]+ (*this)[(x+1)*3][(y)*3+c];
				moyVG = moyVG/2;
				int moyVD=(*this)[(x)*3][(y+1)*3+c]+ (*this)[(x+1)*3][(y+1)*3+c];
				moyVD = moyVD/2;
				//printf("(%d,%d)\n",(i*2+1),(j*2+1));

				x = Hbegin+i;
				y = Vbegin+size/2+j;
				//printf("(%d,%d):%d\n",x,y, to_range(moyVG - moyVD));
				imOut[x*3][y*3+c]= static_cast<unsigned char> (to_range(moyVG - moyVD));
			}
		}

		//printf("D block\n");
		for(int i=0; i<size/2; i++){
			for(int j=0; j<size/2; j++){
				int x= Hbegin + i*2;
				int y= Vbegin + j*2;

				int moyHG=(*this)[x*3][(y)*3+c]+ (*this)[(x+1)*3][(y+1)*3+c];
				moyHG = moyHG;
				int moyHD=(*this)[(x)*3][(y+1)*3+c]+ (*this)[(x+1)*3][(y)*3+c];
				moyHD = moyHD;

				x = Hbegin+size/2+i;
				y = Vbegin+size/2+j;
				//printf("(%d,%d):%d\n",x,y, to_range(moyHG - moyHD));
				imOut[x*3][y*3+c] = static_cast<unsigned char>(to_range(moyHG - moyHD));
			}
		}
	}
	imOut.isValid = 1;
	for(int i=0; i<size; i++){
		for(int j=0; j< size; j++){
			for(int c=0; c<3; c++){
				int x = Hbegin +i;
				int y = Vbegin +j;
				(*this)[x*3][y*3+c]=imOut[x*3][y*3+c];
			}
		}
	}
	printf("transform_vaguellette done\n");
	//this->save("debug_im/in_recurP.ppm");
}

double ImageBase::PSNR(ImageBase target){
	int width= this->getWidth();
	int height = this->getHeight();
	if(target.getWidth() != width || target.getHeight() != height)
	{
		printf("Problem: image are not the same size!\n");
		return 0.0f;
	}
	int diffsum =0;
	for(int c=0; c<3; c++)
		for(int i=0; i<height; i++)
			for(int j=0; j<width; j++){
				diffsum += pow((*this)[i*3][j*3] - target[i*3][j*3+c],2);
			}

	int EQM_color = diffsum/ (height*width);
	if(EQM_color > 0){
		double PSNR = 10.0 * log10(pow(3*255.0,2.0)/(float)EQM_color);
		printf("The PSNR is %lf\n",PSNR);
		return PSNR;
	}
	printf("The images are the same, the PSNR is infinite\n");
	return 0.0f;
}

ImageBase ImageBase::to_RGB(){
	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	int YCrCb[3], RGB[3];
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			YCrCb[0]=(*this)[i*3][j*3+0];
			YCrCb[1]=(*this)[i*3][j*3+1];
			YCrCb[2]=(*this)[i*3][j*3+2];

			YCbCr_to_RGB(YCrCb, RGB);
			check_In_range(RGB);

			imOut[i*3][j*3+0]= static_cast<unsigned char>(RGB[0]);
			imOut[i*3][j*3+1]= static_cast<unsigned char>(RGB[1]);
			imOut[i*3][j*3+2]= static_cast<unsigned char>(RGB[2]);
		}
	}
	return imOut;
}

unsigned char* ImageBase::dictionnaire(ImageBase *imIn, unsigned char * bibliotheque, int taille_tableau) {
	unsigned char* valeurs = imIn->getData();

   std::string sortie;

    for (int i = 0 ; i < imIn->getHeight() * imIn->getWidth() ; i++) {
        for (int j = 0 ; j < taille_tableau ; j++) {

        }
    }

}

ImageBase ImageBase::pallette_CbCr(int k, bool colored){
	//debug
	int nb_boucle=0;

	//palette
	int palette_G[k];
	int palette_B[k];

	//accumulateurs
	int palette_G_sum[k] = {0};
	int palette_B_sum[k] = {0};
	int palette_count[k] = {0};

	//usage interne a boucle
	double minDist, currentDist;
	int currentPalette[2];
	int currentPixel[2];
	int old_color[2];
	int minID, currentID=0;

	//paramètres
	int sqrt_k = sqrt(k);
	int width =  this->getWidth();
	int height = this->getHeight();

	ImageBase imOut(this->getWidth(), this->getHeight(), this->getColor());
	ImageBase paletteOut(sqrt_k,sqrt_k,this->getColor());

	//On initialise la palette avec une grille de points
	for(int i =0; i<sqrt_k; i++){
		for(int j=0; j<sqrt_k; j++){
			//printf("INIT de la palette, currentID: %i\n",currentID);
			palette_G[currentID] = (*this)[i*3* height/sqrt_k][j*3* width/sqrt_k+1];
			palette_B[currentID] = (*this)[i*3* height/sqrt_k][j*3* width/sqrt_k+2];
			currentID++;
		}
	}

	//printf("Begin iteration vers convergence\n");
	bool convergence = false;
	while(!convergence){
		convergence = true;
		//raccorder chaque point a son plus proche
		for(int i=0; i< height; i++){
			for(int j=0; j< width; j++){
				minDist = 257;
				minID = 0;
				currentPixel [1] = (*this)[i*3][j*3+1];
				currentPixel [2] = (*this)[i*3][j*3+2];

				//On cherche la couleur de la palette la plus proche du pixel courant
				for(int k_i=0; k_i < k; k_i++){
					currentPalette[1]= palette_G[k_i];
					currentPalette[2]= palette_B[k_i];

					currentDist = dist(currentPalette, currentPixel);
					if(minDist > currentDist){
						minDist = currentDist;
						minID = k_i;

					}
				}
				palette_G_sum[minID] += currentPixel[1];
				palette_B_sum[minID] += currentPixel[2];
				palette_count[minID] ++;
			}
		}
		//calculer la nouvelle couleur moyenne de chaque ensembles de la palette
		for(int i=0; i<k; i++){
			//On ne considère que les palettes qui ont des points
			if(palette_count[i] != 0){
				old_color[1]= palette_G[i];
				old_color[2]= palette_B[i];

				//Moyenne de la couleur des points approximés à la ieme couleur
				palette_G[i]= palette_G_sum[i]/palette_count[i];
				palette_B[i]= palette_B_sum[i]/palette_count[i];

				//reset de l'ensemble de point approximé à la ieme couleur
				palette_G_sum[i]=0;
				palette_B_sum[i]=0;
				palette_count[i]=0;

				if(	old_color[1] != palette_G[i] ||
					old_color[2] != palette_B[i]){
						convergence= false;
				}
			}
		}
		printf("Nb_boucle: %i\n",nb_boucle++);
	}

	// On met à la couleur correspondante a chaque points:
	for(int i=0; i< height; i++){
			for(int j=0; j< width; j++){
				minDist = 257;
				minID = 0;
				currentPixel [1] = (*this)[i*3][j*3+1];

				//On cherche la couleur de la palette la plus proche du pixel courant
				for(int k_i=0; k_i < k; k_i++){
					currentPalette[1]= palette_G[k_i];

					currentDist = dist(currentPalette, currentPixel);
					if(minDist > currentDist){
						minDist = currentDist;
						minID = k_i;
					}

					if(colored){//send color
						imOut[i*3][j*3+1] = palette_G[minID];
						imOut[i*3][j*3+2] = palette_B[minID];
					}
					else{//send ndg
						imOut[i*3][j*3+1] = minID * 256/k;
					}
				}
			}
		}
	//dump de la palette
	currentID=0;
	for(int i=0; i<sqrt_k; i++){
		for(int j=0; j<sqrt_k; j++){
			paletteOut[i*3][j*3+1] = palette_G[currentID];
			paletteOut[i*3][j*3+2] = palette_B[currentID];
			currentID++;
		}
	}
	paletteOut.save("./Sortie/palette_dump.ppm");

	this->PSNR(imOut);
	return imOut;
}
