#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#ifndef VLC_H
#define VLC_H
uint16_t b2i(const char* bits){
	int res = 0;
	assert(strlen(bits)<=16);
	for(size_t i=0;i<strlen(bits);i++){
		res = res<<1;
		res += '1'== bits[i];
	}
	return res;
}
uint16_t b2l(const char* bits){
	return strlen(bits);
}
struct macroblock_addr_vlc{
	uint16_t len;
	uint16_t incremnet_value;
};
struct macroblock_addr_vlc_init{
	uint16_t bits;
	macroblock_addr_vlc mav;
};
macroblock_addr_vlc_init mavi[] = 
{
	{b2i("1"),{b2l("1"), 1}},
	{b2i("011"),{b2l("011"), 2}},
	{b2i("010"),{b2l("010"), 3}},
	{b2i("0011"),{b2l("0011"), 4}},
	{b2i("0010"),{b2l("0010"), 5}},
	{b2i("00011"),{b2l("00011"), 6}},
	{b2i("00010"),{b2l("00010"), 7}},
	{b2i("0000111"),{b2l("0000111"), 8}},
	{b2i("0000110"),{b2l("0000110"), 9}},
	{b2i("00001011"),{b2l("00001011"), 10}},
	{b2i("00001010"),{b2l("00001010"), 11}},
	{b2i("00001001"),{b2l("00001001"), 12}},
	{b2i("00001000"),{b2l("00001000"), 13}},
	{b2i("00000111"),{b2l("00000111"), 14}},
	{b2i("00000110"),{b2l("00000110"), 15}},
	{b2i("0000010111"),{b2l("0000010111"), 16}},
	{b2i("0000010110"),{b2l("0000010110"), 17}},
	{b2i("0000010101"),{b2l("0000010101"), 18}},
	{b2i("0000010100"),{b2l("0000010100"), 19}},
	{b2i("0000010011"),{b2l("0000010011"), 20}},
	{b2i("0000010010"),{b2l("0000010010"), 21}},
	{b2i("00000100011"),{b2l("00000100011"), 22}},
	{b2i("00000100010"),{b2l("00000100010"), 23}},
	{b2i("00000100001"),{b2l("00000100001"), 24}},
	{b2i("00000100000"),{b2l("00000100000"), 25}},
	{b2i("00000011111"),{b2l("00000011111"), 26}},
	{b2i("00000011110"),{b2l("00000011110"), 27}},
	{b2i("00000011101"),{b2l("00000011101"), 28}},
	{b2i("00000011100"),{b2l("00000011100"), 29}},
	{b2i("00000011011"),{b2l("00000011011"), 30}},
	{b2i("00000011010"),{b2l("00000011010"), 31}},
	{b2i("00000011001"),{b2l("00000011001"), 32}},
	{b2i("00000011000"),{b2l("00000011000"), 33}},
};
macroblock_addr_vlc mav[1<<16];
void init_macroblock_addr_vlc(){
	int array_size = sizeof(mavi)/sizeof(*mavi);
	for(int i=0;i<array_size;i++){
		int t = 16-mavi[i].mav.len;
		for(int j=0;j<(1<<t);j++){
			mav[(mavi[i].bits<<t)|j] = mavi[i].mav;
		}
	}
}

struct macroblock_pattern_vlc{
	uint16_t len;
	uint16_t cbp;
};
struct macroblock_pattern_vlc_init{
	uint16_t bits;
	macroblock_pattern_vlc mpv;
};
macroblock_pattern_vlc_init mpvi[] = 
{
	{b2i("111"),{b2l("111"), 60}},
	{b2i("1101"),{b2l("1101"), 4}},
	{b2i("1100"),{b2l("1100"), 8}},
	{b2i("1011"),{b2l("1011"), 16}},
	{b2i("1010"),{b2l("1010"), 32}},
	{b2i("10011"),{b2l("10011"), 12}},
	{b2i("10010"),{b2l("10010"), 48}},
	{b2i("10001"),{b2l("10001"), 20}},
	{b2i("10000"),{b2l("10000"), 40}},
	{b2i("01111"),{b2l("01111"), 28}},
	{b2i("01110"),{b2l("01110"), 44}},
	{b2i("01101"),{b2l("01101"), 52}},
	{b2i("01100"),{b2l("01100"), 56}},
	{b2i("01011"),{b2l("01011"), 1}},
	{b2i("01010"),{b2l("01010"), 61}},
	{b2i("01001"),{b2l("01001"), 2}},
	{b2i("01000"),{b2l("01000"), 62}},
	{b2i("001111"),{b2l("001111"), 24}},
	{b2i("001110"),{b2l("001110"), 36}},
	{b2i("001101"),{b2l("001101"), 3}},
	{b2i("001100"),{b2l("001100"), 63}},
	{b2i("0010111"),{b2l("0010111"), 5}},
	{b2i("0010110"),{b2l("0010110"), 9}},
	{b2i("0010101"),{b2l("0010101"), 17}},
	{b2i("0010100"),{b2l("0010100"), 33}},
	{b2i("0010011"),{b2l("0010011"), 6}},
	{b2i("0010010"),{b2l("0010010"), 10}},
	{b2i("0010001"),{b2l("0010001"), 18}},
	{b2i("0010000"),{b2l("0010000"), 34}},
	{b2i("00011111"),{b2l("00011111"), 7}},
	{b2i("00011110"),{b2l("00011110"), 11}},
	{b2i("00011101"),{b2l("00011101"), 19}},
	{b2i("00011100"),{b2l("00011100"), 35}},
	{b2i("00011011"),{b2l("00011011"), 13}},
	{b2i("00011010"),{b2l("00011010"), 49}},
	{b2i("00011001"),{b2l("00011001"), 21}},
	{b2i("00011000"),{b2l("00011000"), 41}},
	{b2i("00010111"),{b2l("00010111"), 14}},
	{b2i("00010110"),{b2l("00010110"), 50}},
	{b2i("00010101"),{b2l("00010101"), 22}},
	{b2i("00010100"),{b2l("00010100"), 42}},
	{b2i("00010011"),{b2l("00010011"), 15}},
	{b2i("00010010"),{b2l("00010010"), 51}},
	{b2i("00010001"),{b2l("00010001"), 23}},
	{b2i("00010000"),{b2l("00010000"), 43}},
	{b2i("00001111"),{b2l("00001111"), 25}},
	{b2i("00001110"),{b2l("00001110"), 37}},
	{b2i("00001101"),{b2l("00001101"), 26}},
	{b2i("00001100"),{b2l("00001100"), 38}},
	{b2i("00001011"),{b2l("00001011"), 29}},
	{b2i("00001010"),{b2l("00001010"), 45}},
	{b2i("00001001"),{b2l("00001001"), 53}},
	{b2i("00001000"),{b2l("00001000"), 57}},
	{b2i("00000111"),{b2l("00000111"), 30}},
	{b2i("00000110"),{b2l("00000110"), 46}},
	{b2i("00000101"),{b2l("00000101"), 54}},
	{b2i("00000100"),{b2l("00000100"), 58}},
	{b2i("000000111"),{b2l("000000111"), 31}},
	{b2i("000000110"),{b2l("000000110"), 47}},
	{b2i("000000101"),{b2l("000000101"), 55}},
	{b2i("000000100"),{b2l("000000100"), 59}},
	{b2i("000000011"),{b2l("000000011"), 27}},
	{b2i("000000010"),{b2l("000000010"), 39}},
};
macroblock_pattern_vlc mpv[1<<16];
void init_macroblock_pattern_vlc(){
	int array_size = sizeof(mpvi)/sizeof(*mpvi);
	for(int i=0;i<array_size;i++){
		int t = 16-mpvi[i].mpv.len;
		for(int j=0;j<(1<<t);j++){
			mpv[(mpvi[i].bits<<t)|j] = mpvi[i].mpv;
		}
	}
}
struct motion_vector_vlc{
	uint16_t len;
	int code;
};
struct motion_vector_vlc_init{
	uint16_t bits;
	motion_vector_vlc mvv;
};
motion_vector_vlc_init mvvi[] = 
{
	{b2i("00000011001"),{b2l("00000011001"), -16}},
	{b2i("00000011011"),{b2l("00000011011"), -15}},
	{b2i("00000011101"),{b2l("00000011101"), -14}},
	{b2i("00000011111"),{b2l("00000011111"), -13}},
	{b2i("00000100001"),{b2l("00000100001"), -12}},
	{b2i("00000100011"),{b2l("00000100011"), -11}},
	{b2i("0000010011"),{b2l("0000010011"), -10}},
	{b2i("0000010101"),{b2l("0000010101"), -9}},
	{b2i("0000010111"),{b2l("0000010111"), -8}},
	{b2i("00000111"),{b2l("00000111"), -7}},
	{b2i("00001001"),{b2l("00001001"), -6}},
	{b2i("00001011"),{b2l("00001011"), -5}},
	{b2i("0000111"),{b2l("0000111"), -4}},
	{b2i("00011"),{b2l("00011"), -3}},
	{b2i("0011"),{b2l("0011"), -2}},
	{b2i("011"),{b2l("011"), -1}},
	{b2i("1"),{b2l("1"), 0}},
	{b2i("010"),{b2l("010"), 1}},
	{b2i("0010"),{b2l("0010"), 2}},
	{b2i("00010"),{b2l("00010"), 3}},
	{b2i("0000110"),{b2l("0000110"), 4}},
	{b2i("00001010"),{b2l("00001010"), 5}},
	{b2i("00001000"),{b2l("00001000"), 6}},
	{b2i("00000110"),{b2l("00000110"), 7}},
	{b2i("0000010110"),{b2l("0000010110"), 8}},
	{b2i("0000010100"),{b2l("0000010100"), 9}},
	{b2i("0000010010"),{b2l("0000010010"), 10}},
	{b2i("00000100010"),{b2l("00000100010"), 11}},
	{b2i("00000100000"),{b2l("00000100000"), 12}},
	{b2i("00000011110"),{b2l("00000011110"), 13}},
	{b2i("00000011100"),{b2l("00000011100"), 14}},
	{b2i("00000011010"),{b2l("00000011010"), 15}},
	{b2i("00000011000"),{b2l("00000011000"), 16}},
};
motion_vector_vlc mvv[1<<16];
void init_motion_vector_vlc(){
	int array_size = sizeof(mvvi)/sizeof(*mvvi);
	for(int i=0;i<array_size;i++){
		int t = 16-mvvi[i].mvv.len;
		for(int j=0;j<(1<<t);j++){
			mvv[(mvvi[i].bits<<t)|j] = mvvi[i].mvv;
		}
	}
}
struct dct_coef_lum_vlc{
	uint16_t len;
	uint8_t value;
};
struct dct_coef_lum_vlc_init{
	uint16_t bits;
	dct_coef_lum_vlc dclv;
};
dct_coef_lum_vlc_init dclvi[] = 
{
	{b2i("100"),{b2l("100"), 0}},
	{b2i("00"),{b2l("00"), 1}},
	{b2i("01"),{b2l("01"), 2}},
	{b2i("101"),{b2l("101"), 3}},
	{b2i("110"),{b2l("110"), 4}},
	{b2i("1110"),{b2l("1110"), 5}},
	{b2i("11110"),{b2l("11110"), 6}},
	{b2i("111110"),{b2l("111110"), 7}},
	{b2i("1111110"),{b2l("1111110"), 8}},
};
dct_coef_lum_vlc dclv[1<<16];
void init_dct_coef_lum_vlc(){
	int array_size = sizeof(dclvi)/sizeof(*dclvi);
	for(int i=0;i<array_size;i++){
		int t = 16-dclvi[i].dclv.len;
		for(int j=0;j<(1<<t);j++){
			dclv[(dclvi[i].bits<<t)|j] = dclvi[i].dclv;
		}
	}
}
struct dct_coef_chr_vlc{
	uint16_t len;
	uint16_t value;
};
struct dct_coef_chr_vlc_init{
	uint16_t bits;
	dct_coef_chr_vlc dccv;
};
dct_coef_chr_vlc_init dccvi[] = 
{
	{b2i("00"),{b2l("00"), 0}},
	{b2i("01"),{b2l("01"), 1}},
	{b2i("10"),{b2l("10"), 2}},
	{b2i("110"),{b2l("110"), 3}},
	{b2i("1110"),{b2l("1110"), 4}},
	{b2i("11110"),{b2l("11110"), 5}},
	{b2i("111110"),{b2l("111110"), 6}},
	{b2i("1111110"),{b2l("1111110"), 7}},
	{b2i("11111110"),{b2l("11111110"), 8}},
};
dct_coef_chr_vlc dccv[1<<16];
void init_dct_coef_chr_vlc(){
	int array_size = sizeof(dccvi)/sizeof(*dccvi);
	for(int i=0;i<array_size;i++){
		int t = 16-dccvi[i].dccv.len;
		for(int j=0;j<(1<<t);j++){
			dccv[(dccvi[i].bits<<t)|j] = dccvi[i].dccv;
		}
	}
}
struct dct_coef_f_vlc{
	uint16_t len;
	uint16_t run;
	int level;
};
struct dct_coef_f_vlc_init{
	uint16_t bits;
	dct_coef_f_vlc dcfv;
};
dct_coef_f_vlc_init dcfvi[] = 
{
	{b2i("1"),{b2l("1"), 0, 1}},
	{b2i("011"),{b2l("011"), 1, 1}},
	{b2i("0100"),{b2l("0100"), 0, 2}},
	{b2i("0101"),{b2l("0101"), 2, 1}},
	{b2i("00101"),{b2l("00101"), 0, 3}},
	{b2i("00111"),{b2l("00111"), 3, 1}},
	{b2i("00110"),{b2l("00110"), 4, 1}},
	{b2i("000110"),{b2l("000110"), 1, 2}},
	{b2i("000111"),{b2l("000111"), 5, 1}},
	{b2i("000101"),{b2l("000101"), 6, 1}},
	{b2i("000100"),{b2l("000100"), 7, 1}},
	{b2i("0000110"),{b2l("0000110"), 0, 4}},
	{b2i("0000100"),{b2l("0000100"), 2, 2}},
	{b2i("0000111"),{b2l("0000111"), 8, 1}},
	{b2i("0000101"),{b2l("0000101"), 9, 1}},
	{b2i("000001"),{b2l("000001"), 0, -1}},
	{b2i("00100110"),{b2l("00100110"), 0, 5}},
	{b2i("00100001"),{b2l("00100001"), 0, 6}},
	{b2i("00100101"),{b2l("00100101"), 1, 3}},
	{b2i("00100100"),{b2l("00100100"), 3, 2}},
	{b2i("00100111"),{b2l("00100111"), 10, 1}},
	{b2i("00100011"),{b2l("00100011"), 11, 1}},
	{b2i("00100010"),{b2l("00100010"), 12, 1}},
	{b2i("00100000"),{b2l("00100000"), 13, 1}},
	{b2i("0000001010"),{b2l("0000001010"), 0, 7}},
	{b2i("0000001100"),{b2l("0000001100"), 1, 4}},
	{b2i("0000001011"),{b2l("0000001011"), 2, 3}},
	{b2i("0000001111"),{b2l("0000001111"), 4, 2}},
	{b2i("0000001001"),{b2l("0000001001"), 5, 2}},
	{b2i("0000001110"),{b2l("0000001110"), 14, 1}},
	{b2i("0000001101"),{b2l("0000001101"), 15, 1}},
	{b2i("0000001000"),{b2l("0000001000"), 16, 1}},
	{b2i("000000011101"),{b2l("000000011101"), 0, 8}},
	{b2i("000000011000"),{b2l("000000011000"), 0, 9}},
	{b2i("000000010011"),{b2l("000000010011"), 0, 10}},
	{b2i("000000010000"),{b2l("000000010000"), 0, 11}},
	{b2i("000000011011"),{b2l("000000011011"), 1, 5}},
	{b2i("000000010100"),{b2l("000000010100"), 2, 4}},
	{b2i("000000011100"),{b2l("000000011100"), 3, 3}},
	{b2i("000000010010"),{b2l("000000010010"), 4, 3}},
	{b2i("000000011110"),{b2l("000000011110"), 6, 2}},
	{b2i("000000010101"),{b2l("000000010101"), 7, 2}},
	{b2i("000000010001"),{b2l("000000010001"), 8, 2}},
	{b2i("000000011111"),{b2l("000000011111"), 17, 1}},
	{b2i("000000011010"),{b2l("000000011010"), 18, 1}},
	{b2i("000000011001"),{b2l("000000011001"), 19, 1}},
	{b2i("000000010111"),{b2l("000000010111"), 20, 1}},
	{b2i("000000010110"),{b2l("000000010110"), 21, 1}},
	{b2i("0000000011010"),{b2l("0000000011010"), 0, 12}},
	{b2i("0000000011001"),{b2l("0000000011001"), 0, 13}},
	{b2i("0000000011000"),{b2l("0000000011000"), 0, 14}},
	{b2i("0000000010111"),{b2l("0000000010111"), 0, 15}},
	{b2i("0000000010110"),{b2l("0000000010110"), 1, 6}},
	{b2i("0000000010101"),{b2l("0000000010101"), 1, 7}},
	{b2i("0000000010100"),{b2l("0000000010100"), 2, 5}},
	{b2i("0000000010011"),{b2l("0000000010011"), 3, 4}},
	{b2i("0000000010010"),{b2l("0000000010010"), 5, 3}},
	{b2i("0000000010001"),{b2l("0000000010001"), 9, 2}},
	{b2i("0000000010000"),{b2l("0000000010000"), 10, 2}},
	{b2i("0000000011111"),{b2l("0000000011111"), 22, 1}},
	{b2i("0000000011110"),{b2l("0000000011110"), 23, 1}},
	{b2i("0000000011101"),{b2l("0000000011101"), 24, 1}},
	{b2i("0000000011100"),{b2l("0000000011100"), 25, 1}},
	{b2i("0000000011011"),{b2l("0000000011011"), 26, 1}},
	{b2i("00000000011111"),{b2l("00000000011111"), 0, 16}},
	{b2i("00000000011110"),{b2l("00000000011110"), 0, 17}},
	{b2i("00000000011101"),{b2l("00000000011101"), 0, 18}},
	{b2i("00000000011100"),{b2l("00000000011100"), 0, 19}},
	{b2i("00000000011011"),{b2l("00000000011011"), 0, 20}},
	{b2i("00000000011010"),{b2l("00000000011010"), 0, 21}},
	{b2i("00000000011001"),{b2l("00000000011001"), 0, 22}},
	{b2i("00000000011000"),{b2l("00000000011000"), 0, 23}},
	{b2i("00000000010111"),{b2l("00000000010111"), 0, 24}},
	{b2i("00000000010110"),{b2l("00000000010110"), 0, 25}},
	{b2i("00000000010101"),{b2l("00000000010101"), 0, 26}},
	{b2i("00000000010100"),{b2l("00000000010100"), 0, 27}},
	{b2i("00000000010011"),{b2l("00000000010011"), 0, 28}},
	{b2i("00000000010010"),{b2l("00000000010010"), 0, 29}},
	{b2i("00000000010001"),{b2l("00000000010001"), 0, 30}},
	{b2i("00000000010000"),{b2l("00000000010000"), 0, 31}},
	{b2i("000000000011000"),{b2l("000000000011000"), 0, 32}},
	{b2i("000000000010111"),{b2l("000000000010111"), 0, 33}},
	{b2i("000000000010110"),{b2l("000000000010110"), 0, 34}},
	{b2i("000000000010101"),{b2l("000000000010101"), 0, 35}},
	{b2i("000000000010100"),{b2l("000000000010100"), 0, 36}},
	{b2i("000000000010011"),{b2l("000000000010011"), 0, 37}},
	{b2i("000000000010010"),{b2l("000000000010010"), 0, 38}},
	{b2i("000000000010001"),{b2l("000000000010001"), 0, 39}},
	{b2i("000000000010000"),{b2l("000000000010000"), 0, 40}},
	{b2i("000000000011111"),{b2l("000000000011111"), 1, 8}},
	{b2i("000000000011110"),{b2l("000000000011110"), 1, 9}},
	{b2i("000000000011101"),{b2l("000000000011101"), 1, 10}},
	{b2i("000000000011100"),{b2l("000000000011100"), 1, 11}},
	{b2i("000000000011011"),{b2l("000000000011011"), 1, 12}},
	{b2i("000000000011010"),{b2l("000000000011010"), 1, 13}},
	{b2i("000000000011001"),{b2l("000000000011001"), 1, 14}},
	{b2i("0000000000010011"),{b2l("0000000000010011"), 1, 15}},
	{b2i("0000000000010010"),{b2l("0000000000010010"), 1, 16}},
	{b2i("0000000000010001"),{b2l("0000000000010001"), 1, 17}},
	{b2i("0000000000010000"),{b2l("0000000000010000"), 1, 18}},
	{b2i("0000000000010100"),{b2l("0000000000010100"), 6, 3}},
	{b2i("0000000000011010"),{b2l("0000000000011010"), 11, 2}},
	{b2i("0000000000011001"),{b2l("0000000000011001"), 12, 2}},
	{b2i("0000000000011000"),{b2l("0000000000011000"), 13, 2}},
	{b2i("0000000000010111"),{b2l("0000000000010111"), 14, 2}},
	{b2i("0000000000010110"),{b2l("0000000000010110"), 15, 2}},
	{b2i("0000000000010101"),{b2l("0000000000010101"), 16, 2}},
	{b2i("0000000000011111"),{b2l("0000000000011111"), 27, 1}},
	{b2i("0000000000011110"),{b2l("0000000000011110"), 28, 1}},
	{b2i("0000000000011101"),{b2l("0000000000011101"), 29, 1}},
	{b2i("0000000000011100"),{b2l("0000000000011100"), 30, 1}},
	{b2i("0000000000011011"),{b2l("0000000000011011"), 31, 1}},
};
dct_coef_f_vlc dcfv[1<<16];
void init_dct_coef_f_vlc(){
	int array_size = sizeof(dcfvi)/sizeof(*dcfvi);
	for(int i=0;i<array_size;i++){
		int t = 16-dcfvi[i].dcfv.len;
		for(int j=0;j<(1<<t);j++){
			dcfv[(dcfvi[i].bits<<t)|j] = dcfvi[i].dcfv;
		}
	}
	//printf("%u %d\n",dcfv[0x0400].run,dcfv[0x0400].level);
	//printf("%u %d\n",dcfv[0x8400].run,dcfv[0x8400].level);
	//printf("%u %d\n",dcfv[0x9400].run,dcfv[0x9400].level);
}
struct dct_coef_n_vlc{
	uint16_t len;
	uint16_t run;
	int level;
};
struct dct_coef_n_vlc_init{
	uint16_t bits;
	dct_coef_n_vlc dcnv;
};
dct_coef_n_vlc_init dcnvi[] = 
{
	{b2i("11"),{b2l("11"), 0, 1}},
	{b2i("011"),{b2l("011"), 1, 1}},
	{b2i("0100"),{b2l("0100"), 0, 2}},
	{b2i("0101"),{b2l("0101"), 2, 1}},
	{b2i("00101"),{b2l("00101"), 0, 3}},
	{b2i("00111"),{b2l("00111"), 3, 1}},
	{b2i("00110"),{b2l("00110"), 4, 1}},
	{b2i("000110"),{b2l("000110"), 1, 2}},
	{b2i("000111"),{b2l("000111"), 5, 1}},
	{b2i("000101"),{b2l("000101"), 6, 1}},
	{b2i("000100"),{b2l("000100"), 7, 1}},
	{b2i("0000110"),{b2l("0000110"), 0, 4}},
	{b2i("0000100"),{b2l("0000100"), 2, 2}},
	{b2i("0000111"),{b2l("0000111"), 8, 1}},
	{b2i("0000101"),{b2l("0000101"), 9, 1}},
	{b2i("000001"),{b2l("000001"), 0, -1}},
	{b2i("00100110"),{b2l("00100110"), 0, 5}},
	{b2i("00100001"),{b2l("00100001"), 0, 6}},
	{b2i("00100101"),{b2l("00100101"), 1, 3}},
	{b2i("00100100"),{b2l("00100100"), 3, 2}},
	{b2i("00100111"),{b2l("00100111"), 10, 1}},
	{b2i("00100011"),{b2l("00100011"), 11, 1}},
	{b2i("00100010"),{b2l("00100010"), 12, 1}},
	{b2i("00100000"),{b2l("00100000"), 13, 1}},
	{b2i("0000001010"),{b2l("0000001010"), 0, 7}},
	{b2i("0000001100"),{b2l("0000001100"), 1, 4}},
	{b2i("0000001011"),{b2l("0000001011"), 2, 3}},
	{b2i("0000001111"),{b2l("0000001111"), 4, 2}},
	{b2i("0000001001"),{b2l("0000001001"), 5, 2}},
	{b2i("0000001110"),{b2l("0000001110"), 14, 1}},
	{b2i("0000001101"),{b2l("0000001101"), 15, 1}},
	{b2i("0000001000"),{b2l("0000001000"), 16, 1}},
	{b2i("000000011101"),{b2l("000000011101"), 0, 8}},
	{b2i("000000011000"),{b2l("000000011000"), 0, 9}},
	{b2i("000000010011"),{b2l("000000010011"), 0, 10}},
	{b2i("000000010000"),{b2l("000000010000"), 0, 11}},
	{b2i("000000011011"),{b2l("000000011011"), 1, 5}},
	{b2i("000000010100"),{b2l("000000010100"), 2, 4}},
	{b2i("000000011100"),{b2l("000000011100"), 3, 3}},
	{b2i("000000010010"),{b2l("000000010010"), 4, 3}},
	{b2i("000000011110"),{b2l("000000011110"), 6, 2}},
	{b2i("000000010101"),{b2l("000000010101"), 7, 2}},
	{b2i("000000010001"),{b2l("000000010001"), 8, 2}},
	{b2i("000000011111"),{b2l("000000011111"), 17, 1}},
	{b2i("000000011010"),{b2l("000000011010"), 18, 1}},
	{b2i("000000011001"),{b2l("000000011001"), 19, 1}},
	{b2i("000000010111"),{b2l("000000010111"), 20, 1}},
	{b2i("000000010110"),{b2l("000000010110"), 21, 1}},
	{b2i("0000000011010"),{b2l("0000000011010"), 0, 12}},
	{b2i("0000000011001"),{b2l("0000000011001"), 0, 13}},
	{b2i("0000000011000"),{b2l("0000000011000"), 0, 14}},
	{b2i("0000000010111"),{b2l("0000000010111"), 0, 15}},
	{b2i("0000000010110"),{b2l("0000000010110"), 1, 6}},
	{b2i("0000000010101"),{b2l("0000000010101"), 1, 7}},
	{b2i("0000000010100"),{b2l("0000000010100"), 2, 5}},
	{b2i("0000000010011"),{b2l("0000000010011"), 3, 4}},
	{b2i("0000000010010"),{b2l("0000000010010"), 5, 3}},
	{b2i("0000000010001"),{b2l("0000000010001"), 9, 2}},
	{b2i("0000000010000"),{b2l("0000000010000"), 10, 2}},
	{b2i("0000000011111"),{b2l("0000000011111"), 22, 1}},
	{b2i("0000000011110"),{b2l("0000000011110"), 23, 1}},
	{b2i("0000000011101"),{b2l("0000000011101"), 24, 1}},
	{b2i("0000000011100"),{b2l("0000000011100"), 25, 1}},
	{b2i("0000000011011"),{b2l("0000000011011"), 26, 1}},
	{b2i("00000000011111"),{b2l("00000000011111"), 0, 16}},
	{b2i("00000000011110"),{b2l("00000000011110"), 0, 17}},
	{b2i("00000000011101"),{b2l("00000000011101"), 0, 18}},
	{b2i("00000000011100"),{b2l("00000000011100"), 0, 19}},
	{b2i("00000000011011"),{b2l("00000000011011"), 0, 20}},
	{b2i("00000000011010"),{b2l("00000000011010"), 0, 21}},
	{b2i("00000000011001"),{b2l("00000000011001"), 0, 22}},
	{b2i("00000000011000"),{b2l("00000000011000"), 0, 23}},
	{b2i("00000000010111"),{b2l("00000000010111"), 0, 24}},
	{b2i("00000000010110"),{b2l("00000000010110"), 0, 25}},
	{b2i("00000000010101"),{b2l("00000000010101"), 0, 26}},
	{b2i("00000000010100"),{b2l("00000000010100"), 0, 27}},
	{b2i("00000000010011"),{b2l("00000000010011"), 0, 28}},
	{b2i("00000000010010"),{b2l("00000000010010"), 0, 29}},
	{b2i("00000000010001"),{b2l("00000000010001"), 0, 30}},
	{b2i("00000000010000"),{b2l("00000000010000"), 0, 31}},
	{b2i("000000000011000"),{b2l("000000000011000"), 0, 32}},
	{b2i("000000000010111"),{b2l("000000000010111"), 0, 33}},
	{b2i("000000000010110"),{b2l("000000000010110"), 0, 34}},
	{b2i("000000000010101"),{b2l("000000000010101"), 0, 35}},
	{b2i("000000000010100"),{b2l("000000000010100"), 0, 36}},
	{b2i("000000000010011"),{b2l("000000000010011"), 0, 37}},
	{b2i("000000000010010"),{b2l("000000000010010"), 0, 38}},
	{b2i("000000000010001"),{b2l("000000000010001"), 0, 39}},
	{b2i("000000000010000"),{b2l("000000000010000"), 0, 40}},
	{b2i("000000000011111"),{b2l("000000000011111"), 1, 8}},
	{b2i("000000000011110"),{b2l("000000000011110"), 1, 9}},
	{b2i("000000000011101"),{b2l("000000000011101"), 1, 10}},
	{b2i("000000000011100"),{b2l("000000000011100"), 1, 11}},
	{b2i("000000000011011"),{b2l("000000000011011"), 1, 12}},
	{b2i("000000000011010"),{b2l("000000000011010"), 1, 13}},
	{b2i("000000000011001"),{b2l("000000000011001"), 1, 14}},
	{b2i("0000000000010011"),{b2l("0000000000010011"), 1, 15}},
	{b2i("0000000000010010"),{b2l("0000000000010010"), 1, 16}},
	{b2i("0000000000010001"),{b2l("0000000000010001"), 1, 17}},
	{b2i("0000000000010000"),{b2l("0000000000010000"), 1, 18}},
	{b2i("0000000000010100"),{b2l("0000000000010100"), 6, 3}},
	{b2i("0000000000011010"),{b2l("0000000000011010"), 11, 2}},
	{b2i("0000000000011001"),{b2l("0000000000011001"), 12, 2}},
	{b2i("0000000000011000"),{b2l("0000000000011000"), 13, 2}},
	{b2i("0000000000010111"),{b2l("0000000000010111"), 14, 2}},
	{b2i("0000000000010110"),{b2l("0000000000010110"), 15, 2}},
	{b2i("0000000000010101"),{b2l("0000000000010101"), 16, 2}},
	{b2i("0000000000011111"),{b2l("0000000000011111"), 27, 1}},
	{b2i("0000000000011110"),{b2l("0000000000011110"), 28, 1}},
	{b2i("0000000000011101"),{b2l("0000000000011101"), 29, 1}},
	{b2i("0000000000011100"),{b2l("0000000000011100"), 30, 1}},
	{b2i("0000000000011011"),{b2l("0000000000011011"), 31, 1}},
};
dct_coef_n_vlc dcnv[1<<16];
void init_dct_coef_n_vlc(){
	int array_size = sizeof(dcnvi)/sizeof(*dcnvi);
	for(int i=0;i<array_size;i++){
		int t = 16-dcnvi[i].dcnv.len;
		for(int j=0;j<(1<<t);j++){
			dcnv[(dcnvi[i].bits<<t)|j] = dcnvi[i].dcnv;
		}
	}
	//printf("%u %d\n",dcnv[0x0400].run,dcnv[0x0400].level);
	//printf("%u %d\n",dcnv[0xc400].run,dcnv[0xc400].level);
	//printf("%u %d\n",dcnv[0x9400].run,dcnv[0x9400].level);
}
void init_vlcs(){
	init_macroblock_addr_vlc();
	init_macroblock_pattern_vlc();
	init_motion_vector_vlc();
	init_dct_coef_lum_vlc();
	init_dct_coef_chr_vlc();
	init_dct_coef_f_vlc();
	init_dct_coef_n_vlc();
	return;
}
#endif
