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
	uint32_t slice_start_code;
	uint8_t quantizer_scale;
	uint8_t extra_bit_slice;
	slice(bit_buf* _bb):bb(_bb){}
	void read(){
		slice_start_code = bb->get(32);
		assert(slice_start_code_check(slice_start_code));
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
