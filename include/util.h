#include <cstdio>
#include <cmath>
#ifndef UTIL_H
#define UTIL_H
const double pi = acos(-1);
const double sq8 = sqrt(8);
double cn[5];
double sn[5];
int Clip(int& x,int m,int M){
	if(x>M)x=M;
	if(x<m)x=m;
	return x;
}
inline static void ToRGB(int& y, int& cb, int& cr) {
  cb -= 128; cr -= 128;
  int r = round(y + 1.402 * cr);
  int g = round(y - 0.34414 * cb - 0.71414 * cr);
  int b = round(y + 1.772 * cb);
  y = Clip(r, 0, 255); cb = Clip(g, 0, 255); cr = Clip(b, 0, 255);
}


int Sign(int a){
	return (a>0) - (a<0);
}
void init_idct(){
	for(int i=1;i<=4;i++){
		cn[i] = cos(i*pi/16.0);
		sn[i] = sin(i*pi/16.0);
	}
}
void idct_1D(double n[8][8], double o[8][8], int r){
	double t[8];
	t[0] =  o[r][0]/sq8  + o[r][4]/sq8;
	t[1] = -o[r][4]/sq8  + o[r][0]/sq8;
	t[2] =  o[r][2]/2.0 *sn[2] - o[r][6]/2.0*cn[2];
	t[3] =  o[r][6]/2.0 *sn[2] + o[r][2]/2.0*cn[2];
	t[4] =  o[r][1]/2.0 *cn[1] + o[r][7]/2.0*sn[1];
	t[5] = -o[r][7]/2.0 *cn[1] + o[r][1]/2.0*sn[1];
	t[6] =  o[r][3]/2.0 *cn[3] + o[r][5]/2.0*sn[3];
	t[7] = -o[r][5]/2.0 *cn[3] + o[r][3]/2.0*sn[3];

	n[0][r] =   t[0] +t[3];
	n[1][r] =   t[1] +t[2];
	n[2][r] =  -t[2] +t[1];
	n[3][r] =  -t[3] +t[0];
	n[4][r] =   t[4] +t[6];
	n[5][r] =  (t[5] +t[7]) *cn[4];
	n[6][r] =  (-t[6] +t[4])*cn[4];
	n[7][r] =   -t[7] +t[5];

	t[0] = n[0][r];
	t[1] = n[1][r];
	t[2] = n[2][r];
	t[3] = n[3][r];
	t[4] = n[4][r];
	t[5] = n[5][r] +n[6][r];
	t[6] =-n[6][r] +n[5][r];
	t[7] = n[7][r];
	/*
		 0 1 2 3 4 5 6 7
		 0 1 6 3 7 2 5 4
	 * */	
	n[0][r] =  t[0] +t[4];
	n[1][r] =  t[1] +t[5];
	n[5][r] =  t[2] +t[6];
	n[3][r] =  t[3] +t[7];
	n[7][r] = -t[4] +t[0];
	n[6][r] = -t[5] +t[1];
	n[2][r] = -t[6] +t[2];
	n[4][r] = -t[7] +t[3];
}
void idct(int block[][8]){
	puts("IDCT!!");
	double d_block[2][8][8];
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++) d_block[0][i][j] = block[i][j];
	for(int i=0;i<8;i++){
		idct_1D(d_block[1],d_block[0],i);
	}
	for(int i=0;i<8;i++)
		idct_1D(d_block[0],d_block[1],i);
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++) block[i][j] = d_block[0][i][j];
}

#endif
