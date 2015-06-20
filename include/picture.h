#include <cstdio>
#include <bit_buf.h>
#include <start_codes.h>
#include <cassert>
#ifndef PICTURE_H
#define PICTURE
struct picture{
	bit_buf* bb;
	uint32_t picture_start_code;
	uint16_t temporal_reference;
	uint8_t picture_coding_type;
	uint16_t vbv_delay;
	uint8_t full_pel_forward_vector;
	uint8_t full_pel_backward_vector;
	uint8_t forward_f_code;
	uint8_t backward_f_code;
	picture(bit_buf* _bb):bb(_bb){}
	void read(){
		picture_start_code = bb->get(32);
		assert(picture_start_code == PICTURE_START_CODE);
		temporal_reference = bb->get(10);
		picture_coding_type = bb->get(3);
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
		uint32_t tmp;
		tmp = bb->nextbits(32);
		printf("%x\n",tmp);

	}
};
#endif
