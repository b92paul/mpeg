#include <cstdio>
#include <bit_buf.h>
#include <start_codes.h>
#include <cassert>
#include <vector>
#include <util.h>
using namespace std;
#ifndef PICTURE_H
#define PICTURE
struct picture{
	bit_buf* bb;
	sequence_header* sh;
	int width, height;
	uint32_t picture_start_code;
	uint16_t temporal_reference;
	uint8_t picture_coding_type;
	uint16_t vbv_delay;
	uint8_t full_pel_forward_vector;
	uint8_t full_pel_backward_vector;
	uint8_t forward_f_code;
	uint8_t backward_f_code;
	int past_intra_address;
	vector<vector<int> > img[3];
	picture(bit_buf* _bb, sequence_header* _sh):bb(_bb),sh(_sh){}
	void final_picture(int real_width,int real_height){
		for(int i = real_height-1;i>=0;i--){
			for(int j=real_width-1;j>=0;j--){
				img[1][i][j] = img[1][i/2][j/2];
				img[2][i][j] = img[2][i/2][j/2];
				ToRGB(img[0][i][j],img[1][i][j],img[2][i][j]);	
			}
		}
		width = real_width;
		height = real_height;
	}
	void read(){
		for(int i=0;i<3;i++){
			img[i].resize(sh->mb_height*16);
			for(int j=0;j<sh->mb_height*16;j++){
				img[i][j].resize(sh->mb_width*16,0); 
			}
		}
		width = sh->mb_width*16;
		height = sh->mb_height*16;
		printf("img size:%lu %lu\n",img[0][0].size(),img[0].size());
		picture_start_code = bb->get(32);
		assert(picture_start_code == PICTURE_START_CODE);
		temporal_reference = bb->get(10);
		picture_coding_type = bb->get(3);
		printf("type: %d\n",picture_coding_type);
		vbv_delay = bb->get(16);
		if(picture_coding_type == 2 || picture_coding_type == 3){
			full_pel_forward_vector = bb->get(1);
			forward_f_code = bb->get(3);
		}
		if(picture_coding_type == 3){
			full_pel_backward_vector = bb->get(1);
			backward_f_code = bb->get(3);
		}
		uint8_t extra_bit_picture = bb->get(1);
		assert(extra_bit_picture == 0);
		bb->next_start_code();
		if(bb->nextbits() == EXTENSION_START_CODE){
			bb->get(32);
			while(bb->nextbits()!=0x000001){
				bb->get(8);
			}
			bb->next_start_code();
		}
		if(bb->nextbits() == USER_DATA_START_CODE){
			bb->get(32);
			while(bb->nextbits()!=0x000001){
				bb->get(8);	
			}
			bb->next_start_code();
		}
	}
};
#endif
