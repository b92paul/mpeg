#include <cstdio>
#include <bit_buf.h>
#include <vlc.h>
#include <util.h>
#include <cstring>
#ifndef MACROBLOCK_H
#define MACROBLOCK_H
static int scan[8][8] = {
	{0,1,5,6,14,15,27,28},
	{2,4,7,13,16,26,29,42},
	{3,8,12,17,25,30,41,43},
	{9,11,18,24,31,40,44,53},
	{10,19,23,32,39,45,52,54},
	{20,22,33,38,46,51,55,60},
	{21,34,37,47,50,56,59,61},
	{35,36,48,49,57,58,62,63}
};
struct macroblock{
	bit_buf* bb;
	uint8_t ptype;
	picture* p;
	sequence_header* sh;
	slice* slice_now;
	macroblock_type_vlc mt;
	uint8_t macro_addr_incr;
	uint8_t quantizer_scale;
	uint8_t cbp;
	bool pattern_code[6];
	uint16_t peek;
	uint8_t len;
	int macroblock_address;
	macroblock(bit_buf* _bb,sequence_header* _sh, slice* _slice_now, picture* _p, uint8_t _ptype)
		:bb(_bb), sh(_sh), slice_now(_slice_now), p(_p),ptype(_ptype){}
	void to_zig_zag(int dct_zz[64],int dct_recon[8][8]){
		for(int m=0;m<8;m++)
			for(int n=0;n<8;n++)
				dct_recon[m][n] = dct_zz[scan[m][n]];
	}
	void block(int i){
		int dct_zz[64];
		memset(dct_zz,0,sizeof(dct_zz));
		printf("block %d\n",i);
		if(pattern_code[i]){
			int idx = 0;
			if(mt.macroblock_intra){
				if(i<4){
					peek = bb->nextbits(16);
					len = dclv[peek].len;
					bb->get(len);
					uint8_t dct_lum_size = dclv[peek].value;
					int diff = 0;
					if(dct_lum_size !=0){
						diff =(int)bb->get(dct_lum_size);
						if(diff & (1<<(dct_lum_size-1))) dct_zz[0] = diff;
						else dct_zz[0] = (-1<<(dct_lum_size))|(diff+1);
					}
				}
				else{
					peek = bb->nextbits(16);
					len = dccv[peek].len;
					bb->get(len);
					uint8_t dct_chr_size = dccv[peek].value;
					//printf("%u\n",dct_chr_size);
					int diff = 0;
					if(dct_chr_size != 0){
						diff = (int)bb->get(dct_chr_size);
						if(diff & (1<<(dct_chr_size-1))) dct_zz[0] = diff;
						else dct_zz[0] = (-1<<(dct_chr_size))|(diff+1);
					}
				}
			}
			// read dct_first if ptype not intra
			else{
				peek = bb->nextbits(16);
				len = dcnv[peek].len;
				uint16_t run = dcnv[peek].run;
				int level = dcnv[peek].level;
				if(level != -1){
					bb->get(len);
					if(bb->get(1))level = -level;
				}
				else{
					assert(len==6);
					bb->get(len);
					run = bb->get(6);
					uint32_t flc = bb->get(8);
					if(flc == 0x80){
						flc = bb->get(8);
						level = (int)flc - 256;
					}
					else if(flc == 0x00){
							flc = bb->get(8);
							level = flc;
					}
					else{
						if(flc& 0x80) level= (int)flc -256;
						else level = flc;
					}
				}
				idx = run;
				dct_zz[idx] = level;
			}
			//read dct_next
			if(ptype+1 != 4){
				while(bb->nextbits(2) != 0x2){
					peek = bb->nextbits(16);
					len = dcnv[peek].len;
					uint16_t run = dcnv[peek].run;
					int level = dcnv[peek].level;
					//printf("len:%d dcnv run %u level %d\n",len,run,level);
					if(level != -1){
						bb->get(len);
						if(bb->get(1)) level = -level;
					}
					else{
						assert(len==6);
						bb->get(len);
						run = bb->get(6);
						uint32_t flc = bb->get(8);
						if(flc == 0x80){
							flc = bb->get(8);
							level = (int)flc -256;
						}
						else if(flc == 0x00){
							flc = bb->get(8);
							level = flc;
						}
						else{
							if(flc& 0x80) level= (int)flc - 256;
							else level = flc;
						}
					}
					idx += run+1;
					assert(idx<64);
					dct_zz[idx] = level;
				}
				bb->get(2);
			}
			block_data(i,dct_zz);
		}	
	}
	void block_data(int i,int dct_zz[64]){
		int dct_recon[8][8];
		int bx,by;
		if(i<4){
			bx = sh->mb_column * 16 + (i&1)*8;
			by = sh->mb_row * 16 + (i/2)*8;
		} else{
			bx = sh->mb_column *8;
			by = sh->mb_row * 8;
		}
		// I frame
		if(ptype==0){
			to_zig_zag(dct_zz, dct_recon);
			for(int m=0;m<8;m++){
				for(int n=0;n<8;n++){
					int& d = dct_recon[m][n];
					d = (2*d*(quantizer_scale)*(sh->intra_quantizer_matrix[m][n]))/16;
					if((d&1)==0)
						d = d-Sign(d);
						Clip(d,-2048,2047);
				}
			}
			if(i<4){
				printf("dct_past:%d\n",sh->dct_dc_y_past);
				// lum
				if(i==0){
					dct_recon[0][0] = dct_zz[0]*8;
					if((macroblock_address - sh->past_intra_address)>1){
						dct_recon[0][0] = 128*8 + dct_recon[0][0];
					}
					else
						dct_recon[0][0] = sh->dct_dc_y_past + dct_recon[0][0];	
					sh->dct_dc_y_past = dct_recon[0][0];
				}
				else{
					dct_recon[0][0] = sh->dct_dc_y_past + dct_zz[0]*8;
					sh->dct_dc_y_past = dct_recon[0][0];
				}
			} else if(i==4){ 
				// chr
				dct_recon[0][0] = dct_zz[0]*8;
				if((macroblock_address - sh->past_intra_address)>1)
					dct_recon[0][0] = 128*8 + dct_recon[0][0];
				else
					dct_recon[0][0] = sh->dct_dc_cb_past + dct_recon[0][0];
				sh->dct_dc_cb_past = dct_recon[0][0];
			} else if(i==5){
				dct_recon[0][0] = dct_zz[0]*8;
				if((macroblock_address - sh->past_intra_address)>1)
					dct_recon[0][0] = 128*8 + dct_recon[0][0];
				else
					dct_recon[0][0] = sh->dct_dc_cr_past + dct_recon[0][0];
				sh->dct_dc_cr_past = dct_recon[0][0];
			}
			else assert(false);
			idct(dct_recon);
			if(i<4){
				for(int y=0;y<8;y++){
					for(int x=0;x<8;x++){
						p->img[0][by+y][bx+x] = Clip(dct_recon[y][x],0,255);
					}
				}
			} else{
				for(int y=0;y<8;y++){
					for(int x=0;x<8;x++)
						p->img[i-4+1][by+y][bx+x] = Clip(dct_recon[y][x],0,255);
				}
			}
			//printf("past_intra_addres: %d\n",sh->past_intra_address);
			//printf("dct_recon[0][0]:%d\n",dct_recon[0][0]);
		}
		else{
			//TODO P B
			assert(false);
		}
	}
	int macroblock_escape;
	void read(){
		macroblock_escape = 0;
		while(bb->nextbits(11)==0x00f){
			puts("read stuffing!");
			bb->get(11);
		}
		while(bb->nextbits(11)==0x008){
			puts("read stuffing!");
			bb->get(11);
			macroblock_escape ++;
		}
		//incr
		peek = bb->nextbits(16);
		len = mav[peek].len; //get_len
		bb->get(len);
		macro_addr_incr = mav[peek].incr_value + macroblock_escape * 33;
		macroblock_address = sh->pre_macroblock_address + macro_addr_incr;
		
		printf("incr:%d\n",macro_addr_incr);
		printf("macroblock_address:%d\n",macroblock_address);

		sh->mb_row = macroblock_address/ sh->mb_width;
		sh->mb_column = macroblock_address % sh->mb_width;
		printf("%d %d\n",sh->mb_row,sh->mb_column);
		//type
		peek = bb->nextbits(16);
		mt = mtv[ptype][peek];
		len = mtv[ptype][peek].len;
		bb->get(len);
		//reset past
		if(!mt.macroblock_intra)
			sh->dct_dc_y_past = sh->dct_dc_cb_past = sh->dct_dc_cr_past = 1024;


		quantizer_scale = slice_now -> quantizer_scale;
		if(mt.macroblock_quant){
			quantizer_scale = bb->get(5);
			slice_now ->quantizer_scale = quantizer_scale;
		}
		if(mt.macroblock_motion_forward){
			assert(false);
		}
		if(mt.macroblock_motion_backward){
			assert(false);
		}
		assert(mt.macroblock_intra);
		if(mt.macroblock_pattern){
			assert(false);
			peek = bb->get(16);
			len = mpv[peek].len;
			cbp = mpv[peek].cbp;
		}
		for(int i=0;i<6;i++){
			if(cbp&(1<<(5-i))) pattern_code[i] = true;
			if(mt.macroblock_intra) pattern_code[i] = true;
			block(i);
		}

		if(ptype+1 == 4)
			bb->get(1);
		sh->pre_macroblock_address = macroblock_address;
		if(mt.macroblock_intra)
			sh->past_intra_address = macroblock_address;
	};
};
#endif
