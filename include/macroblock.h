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
	int macro_addr_incr;
	uint8_t quantizer_scale;
	uint8_t cbp;
	bool pattern_code[6];
	uint16_t peek;
	uint8_t len;
	int macroblock_address;
	int mo_h_f_code, mo_v_f_code;
	int mo_h_f_r, mo_v_f_r;
	int mo_h_b_code, mo_v_b_code;
	int mo_h_b_r, mo_v_b_r;
	bool skipping=false;
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
		//printf("block %d\n",i);
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
				assert(!skipping);
				peek = bb->nextbits(16);
				len = dcfv[peek].len;
				uint16_t run = dcfv[peek].run;
				int level = dcfv[peek].level;
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
				assert(idx<64);
				bb->get(2);
			}
		}	
		block_data(i,dct_zz);
	}
	void block_data(int i,int dct_zz[64]){
		int dct_recon[8][8];
		to_zig_zag(dct_zz, dct_recon);
		int bx,by;
		if(i<4){
			bx = sh->mb_column * 16 + (i&1)*8;
			by = sh->mb_row * 16 + (i/2)*8;
		} else{
			bx = sh->mb_column *8;
			by = sh->mb_row * 8;
		}
		// I frame
		if(mt.macroblock_intra){
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
				//printf("dct_past:%d\n",sh->dct_dc_y_past);
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
					for(int x=0;x<8;x++){
						p->img[i-4+1][by+y][bx+x] = Clip(dct_recon[y][x],0,255);
					}
				}
			}
		}
		else if(ptype ==1 ){
			for(int m=0;m<8;m++){
				for(int n=0;n<8;n++){
					int&d = dct_recon[m][n];
					if(d==0)continue;
					d = ( (2*d+Sign(d)) *quantizer_scale*(sh->non_intra_quantizer_matrix[m][n]))/16;
					if((d&1) == 0 ) d -= Sign(d);
					d = Clip(d,-2048,2047);
				}
			}
			idct(dct_recon);
			int right_for, right_half_for;
			int down_for, down_half_for;
			if(i<4){
				right_for = sh->recon_right_for >> 1;
				down_for  = sh->recon_down_for >> 1;
				right_half_for = sh->recon_right_for - 2*right_for;
				down_half_for  = sh->recon_down_for  - 2*down_for;
				//printf("%d %d\n",right_for,down_for);
				//printf("%d %d\n",right_half_for,down_half_for);
				assert(sh->picture_ref1!=NULL);
				for(int y=0;y<8;y++){
					for(int x = 0;x<8;x++){
						dct_recon[y][x] += get_half(sh->picture_ref1->img[0],bx+x+right_for,by+y+down_for,0,0);
						p->img[0][by+y][bx+x] = Clip(dct_recon[y][x], 0,255);
					}
				}
			} else{
				right_for = (sh->recon_right_for/2) >> 1;
				down_for = (sh->recon_down_for/2) >> 1;
				right_half_for = sh->recon_right_for/2 - 2*right_for;
				down_half_for = sh->recon_down_for/2 - 2*down_for;
				for(int y=0; y<8; y++){
					for(int x = 0; x<8; x++){
						dct_recon[y][x] +=get_half(sh->picture_ref1->img[i-4+1],bx+x+right_for,by+y+down_for,0,0);
						//printf("%d %d %d ",y+by,x+bx,sh->picture_ref1->img[i-4+1][by+y][bx+x]);
						p->img[i-4+1][by+y][bx+x] = Clip(dct_recon[y][x],0,255);
					}
				}//puts("");
			}
		} else if(ptype ==2){
			//TODO B
			assert(false);
		
		}
	}
	int get_half(vector<vector<int>>&data, int x, int y, int hx, int hy){
		if(!hx){
			if(!hy) return data[y][x];
			else return div_round(data[y][x] + data[y+1][x],2);
		}	else{
			if(!hy) return div_round(data[y][x] + data[y][x+1],2);
			else return div_round(data[y][x]+data[y+1][x]+data[y][x+1]+data[y+1][x+1],4);
		}
	}
	int macroblock_escape=0;

	void skip_macroblock(){
		if(ptype == 0) assert(false);
		else if(ptype ==1){
			sh->recon_right_for_pre = sh->recon_down_for_pre = 0;
			sh->recon_right_for = sh->recon_down_for = 0;
			int dct_zz[64];
			memset(dct_zz,0,sizeof(dct_zz));
			assert(mt.macroblock_intra==false);
			for(int i=0;i<6;i++){
				block_data(i,dct_zz);
			}
			sh->dct_dc_y_past = sh->dct_dc_cb_past = sh->dct_dc_cr_past = 1024;
		}
		else if(ptype ==2){
			int dct_zz[64];
			memset(dct_zz,0,sizeof(dct_zz));
			for(int i=0;i<6;i++){
				block_data(i,dct_zz);
			}
			sh->dct_dc_y_past = sh->dct_dc_cb_past = sh->dct_dc_cr_past = 1024;
		}
	};
	void read(){
		macroblock_escape = 0;
		while(bb->nextbits(11)==0x00f){
			//puts("read stuffing!");
			bb->get(11);
		}
		while(bb->nextbits(11)==0x008){
			//puts("read escape!");
			bb->get(11);
			macroblock_escape ++;
		}
		//incr
		uint16_t tmp = bb->nextbits(16);
		peek = bb->nextbits(16);
		len = mav[peek].len; //get_len
		bb->get(len);
		macro_addr_incr = mav[peek].incr_value + macroblock_escape * 33;
		macroblock_address = sh->pre_macroblock_address + macro_addr_incr;
		int addr_tmp = macroblock_address;
		skipping = true;
		for(int addr = sh->pre_macroblock_address+1; addr < macroblock_address; addr++){
			assert(addr>=0);
			sh->mb_row = addr/ sh->mb_width;
			sh->mb_column = addr % sh->mb_width;
			mt.macroblock_intra = 0;
			skip_macroblock();
		}
		skipping = false;
		
		sh->mb_row = macroblock_address/ sh->mb_width;
		sh->mb_column = macroblock_address % sh->mb_width;
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
			peek = bb->nextbits(16);
			//printf("%x\n",peek);
			len = mvv[peek].len;
			bb->get(len);
			mo_h_f_code = mvv[peek].code;
			//printf("forward_r_size %d\n",p->forward_r_size);
			if( (p->forward_f!=1) && (mo_h_f_code!=0)){
				mo_h_f_r = bb->get(p->forward_r_size);
			}
			peek = bb->nextbits(16);
			len = mvv[peek].len;
			bb->get(len);
			mo_v_f_code = mvv[peek].code;
			if( (p->forward_f!=1) && (mo_v_f_code!=0))
				mo_v_f_r = bb->get(p->forward_r_size);
			//assert(false);
			calc_forward_vector();
		}
		else{
			if(ptype == 1){
				sh->recon_right_for = sh->recon_down_for = 0;
			}
		}
		if(mt.macroblock_motion_backward){
			peek = bb->nextbits(16);
			len = mvv[peek].len;
			bb->get(len);
			mo_h_b_code = mvv[peek].code;
			//printf("backward_r_size %d\n",p->backward_r_size);
			if( (p->backward_f!=1) && (mo_h_b_code!=0)){
				mo_h_b_r = bb->get(p->backward_r_size);
			}
			peek = bb->nextbits(16);
			len = mvv[peek].len;
			bb->get(len);
			mo_v_b_code = mvv[peek].code;
			if( (p->backward_f!=1) && (mo_v_b_code!=0))
				mo_v_b_r = bb->get(p->backward_r_size);
		}
		//assert(mt.macroblock_intra);
		cbp = 0;
		if(mt.macroblock_pattern){
			peek = bb->nextbits(16);
			len = mpv[peek].len;
			cbp = mpv[peek].cbp;
			bb->get(len);
		}
		for(int i=0;i<6;i++){
			pattern_code[i] = 0;
			if(cbp&(1<<(5-i))) pattern_code[i] = true;
			if(mt.macroblock_intra) pattern_code[i] = true;
			block(i);
		}

		if(ptype+1 == 4)
			bb->get(1);
		sh->pre_macroblock_address = macroblock_address;
		if(mt.macroblock_intra)
			sh->past_intra_address = macroblock_address;
		if(ptype==1){
			if(!mt.macroblock_motion_forward){
				sh->recon_right_for_pre = sh->recon_down_for_pre = 0; 
			} 
		} else if(ptype==2){
		
		}
	}
	void calc_forward_vector(){
		int	complement_horizontal_forward_r, complement_vertical_forward_r;
		int forward_f = p->forward_f;
		if(forward_f ==1 || mo_h_f_code == 0){
			complement_horizontal_forward_r = 0;
		} else{
			complement_horizontal_forward_r = forward_f -1 -mo_h_f_r;
		}
		if(forward_f ==1 || mo_v_f_code == 0){
			complement_vertical_forward_r = 0;
		} else{
			complement_vertical_forward_r = forward_f -1 -mo_v_f_r;
		}
		int right_little = mo_h_f_code * forward_f, right_big;
		if(right_little==0){
			right_big = 0;
		} else{
			if(right_little>0){
				right_little = right_little - complement_horizontal_forward_r;
				right_big = right_little - 32*forward_f;
			} else{
				right_little = right_little + complement_horizontal_forward_r;
				right_big = right_little +	32*forward_f;
			}
		}
		assert(right_little!=forward_f*16);
		int down_little = mo_v_f_code * forward_f, down_big;
		if(down_little ==0){
			down_big = 0;
		}	else{
			if(down_little>0){
				down_little = down_little - complement_vertical_forward_r;
				down_big = down_little - 32*forward_f;
			} else{
				down_little = down_little + complement_vertical_forward_r;
				down_big = down_little + 32*forward_f;
			}
		}
		assert(down_little != forward_f*16);
		int& recon_right_for = sh->recon_right_for;
		int& recon_right_for_pre = sh->recon_right_for_pre;
		int& recon_down_for = sh-> recon_down_for;
		int& recon_down_for_pre = sh->recon_down_for_pre;

		int Max = ( 16*forward_f)-1;
		int Min = (-16*forward_f);
		int new_vector;
		new_vector = recon_right_for_pre + right_little;
		if(new_vector<=Max && new_vector>=Min)
			recon_right_for = recon_right_for_pre + right_little;
		else
			recon_right_for = recon_right_for_pre + right_big;
		recon_right_for_pre = recon_right_for;
		if(p->full_pel_forward_vector) recon_right_for <<=1;
		
		new_vector = recon_down_for_pre + down_little;
		if(new_vector<=Max && new_vector>=Min)
			recon_down_for = recon_down_for_pre + down_little;
		else
			recon_down_for = recon_down_for_pre + down_big;
		recon_down_for_pre = recon_down_for;
		if(p->full_pel_forward_vector) recon_down_for <<=1;
		//new_vector = recon_right
	}

};
#endif
