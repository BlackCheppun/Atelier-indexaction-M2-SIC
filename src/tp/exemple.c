#include<stdio.h>
#include<stdlib.h>

#include "def.h"
#include "nrio.h"
#include "nrarith.h"
#include "nralloc.h"



int main(void){

	long nrh,nrl,nch,ncl;
	byte **I;
	byte **R;


	
	int i,j;
	
	I=LoadPGM_bmatrix("images/cubes.pgm",&nrl,&nrh,&ncl,&nch);
	
	
	R=bmatrix(nrl,nrh,ncl,nch);


	SavePGM_bmatrix(I,nrl,nrh,ncl,nch,"resultats/imageTest.pgm");
	
	free_bmatrix(I,nrl,nrh,ncl,nch);


	return 1;

}
