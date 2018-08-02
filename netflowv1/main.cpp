/************************************************************************
NetFlowV1
Program to compute flow in microvascular networks, including in-vivo
viscosity law and hematocrit partition in bifurcations.
See Pries and Secomb 2008, Handbook of Physiology, for formulas.
Reads Network.dat file, computes flows, and writes NetworkNew.dat.
Note that only segments with segtyp = 4 or 5 in network.dat are included.
This allows segments to be excluded from the network easily.
TWS, September 2012
************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"

void input();
void analyzenet();
void setuparrays1(int nseg, int nnod);
void flow();
void writeflow();
void picturenetwork(float *nodvar, float *segvar, const char fname[]);

int max=100,nseg,nnod,nnodbc,niter,nnodfl,nsegfl,mxx,myy,mzz,nodsegm;
int nitmax1,nitmax,varyviscosity,phaseseparation;
int *segtyp,*segname,*bcnodname,*bcnod,*bctyp,*nodname,*nodtyp;
int *nodrank,*nodout,*nk,*ista,*iend;
int **nodnod,**nodseg,**segnodname;

float pi1 = atan(1.0)*4.0;
float facfp,vplas,mcvcorr,alx,aly,alz,lb,maxl;
float constvisc,consthd,tol,qtol,hdtol,omega,optw,optlam;
float *diam,*q,*qinput,*hd,*hdinput,*bcprfl,*bchd,*lseg,*qold,*hdold,*cond;
float *bifpar,*cpar,*viscpar,*segvar,*nodvar;
float **cnode;
double *nodpress;

int main(int argc, char *argv[])
{
	int iseg,inod;

	input();	//read data files

	setuparrays1(nseg,nnod);

	analyzenet();

	flow();

	writeflow();	//creat new network.dat file called 'NetworkNew.dat'

	//make pictures of the network showing distributions of flow, pressure and hematocrit

	for(inod=1; inod<=nnod; inod++) nodvar[inod] = inod;
	for(iseg=1; iseg<=nseg; iseg++) segvar[iseg] = 0.;
	for(iseg=1; iseg<=nseg; iseg++) if(segtyp[iseg] == 4 || segtyp[iseg] == 5) segvar[iseg] = fabs(q[iseg]);
	picturenetwork(nodvar,segvar,"networkflow.ps");
	for(iseg=1; iseg<=nseg; iseg++) if(segtyp[iseg] == 4 || segtyp[iseg] == 5) segvar[iseg] = (nodpress[ista[iseg]] + nodpress[iend[iseg]])/2.;
	picturenetwork(nodvar,segvar,"networkpressure.ps");
	for(iseg=1; iseg<=nseg; iseg++) if(segtyp[iseg] == 4 || segtyp[iseg] == 5) segvar[iseg] = hd[iseg];
	picturenetwork(nodvar,segvar,"networkhemat.ps");

	return 0;
}