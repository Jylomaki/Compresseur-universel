/******************************************************************************
* ICAR_Library
*
* Fichier : ImageBase.h
*
* Description : Classe contennant quelques fonctionnalit�s de base
*
* Auteur : Mickael Pinto
*
* Mail : mickael.pinto@live.fr
*
* Date : Octobre 2012
*
*******************************************************************************/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

class ImageBase
{
	///////////// Enumerations
public:
	typedef enum { PLAN_R, PLAN_G, PLAN_B} PLAN;


	///////////// Attributs
protected:
	unsigned char *data;
	double *dataD;

	bool color;
	int height;
	int width;
	int nTaille;
	bool isValid;


	///////////// Constructeurs/Destructeurs
protected:
	void init();
	void reset();

public:
	ImageBase(void);
	ImageBase(int imWidth, int imHeight, bool isColor);
	~ImageBase(void);

	///////////// Methodes
protected:
	void copy(const ImageBase &copy);

public:
	int getHeight() { return height; };
	int getWidth() { return width; };
	int getTotalSize() { return nTaille; };
	int getValidity() { return isValid; };
	bool getColor() { return color; };
	unsigned char *getData() { return data; };

	ImageBase threshold(int S);
	ImageBase threshold4(int S1, int S2, int S3);
	ImageBase thresholdPPM(int SR, int SG, int SB);
	ImageBase thresholdFond(int S);

	void profilpgmLC(char *lc, int index);
	void histPGM();
	void histPPM();

	int* histo();
	int* compareTab(int* tab1, int* tab2);

	ImageBase getImg();

	ImageBase erosion();
	ImageBase erosionPPM();
	ImageBase erosionPGM();
	ImageBase dilatation();
	ImageBase dilatationPPM();
	ImageBase dilatationPGM();
	ImageBase fermeture();
	ImageBase fermeturePPM();
	ImageBase fermeturePGM();
	ImageBase ouverture();
	ImageBase ouverturePPM();
	ImageBase ouverturePGM();

	ImageBase segmentation(ImageBase seuillee, ImageBase dilatee);

	ImageBase expansionDynamique();
	ImageBase expansionDynamiquePPM();

	ImageBase thresholdPPMhist();

	ImageBase greyLevel();

	ImageBase flouCouleur();

	ImageBase flouFond(ImageBase fond);

	ImageBase moyenne4Region();

	void recursRegion(ImageBase &imOut, int idepart, int jdepart, int taille, int S);

	void load(char *filename);
	bool save(char *filename);

	ImageBase *getPlan(PLAN plan);

	unsigned char *operator[](int l);
	double dist(int c1[3], int c2[3]);

	ImageBase deux_mean();
	ImageBase K_mean(int k, bool colored);
	ImageBase TP2_reechantillonage(int canal1, int canal2, int to_dump);
	ImageBase TP2_reechantillonage_YUV(int to_dump);
	bool validCoordinate(int x, int y, int h, int w);
	void RGB_to_YCbCr(int *RBG, int *YCbCbr);
	void YCbCr_to_RBG(int *YCbCbr, int *RGB);
	ImageBase to_YCrCb();
	ImageBase to_RGB();
	void check_In_range(int* RGB);
	int to_range(int C);

	void transform_vaguellette(int n,int Hbegin, int Vbegin, int size);
	void transform_vaguellette(int Hbegin, int Vbegin, int size);
	double PSNR(ImageBase target);


};
