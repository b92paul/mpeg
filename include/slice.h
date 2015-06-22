#include <cstdio>
#include <bit_buf.h>
#include <start_codes.h>
#include <cassert>

#ifndef SLICE_H
#define SLICE_H
bool slice_start_code_check(uint32_t code){
	code = code ^ 0x00000100;
	return code<=(uint32_t)0xaf;
}
struct slice{
	bit_buf* bb;
	sequence_header* sh;
	uint32_t slice_start_code;
	uint8_t quantizer_scale;
	uint8_t extra_bit_slice;
	uint8_t slice_vertical_position;
	slice(bit_buf* _bb,sequence_header* _sh):bb(_bb),sh(_sh){}
	void read(){
		sh->dct_dc_y_past = sh->dct_dc_cb_past = sh->dct_dc_cr_past = 1024;
		sh->past_intra_address = -2;
		slice_start_code = bb->get(32);
		assert(slice_start_code_check(slice_start_code));
		slice_vertical_position = slice_start_code & 0xff;
		sh->pre_macroblock_address=(slice_vertical_position-1)*(sh->mb_width)-1;
		printf("pre_address!!!!!!!!!!!:%d\n",sh->pre_macroblock_address);
		quantizer_scale = bb->get(5);
		while( bb->nextbits() == 1){
			extra_bit_slice = bb->get(1);
			assert(extra_bit_slice == 1);
			bb->get(8);
		}
		extra_bit_slice = bb->get(1);
		assert(extra_bit_slice == 0);

	}
};
#endif
