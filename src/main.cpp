#include <cstdio>
#include <bit_buf.h>
#include <sequence_header.h>
#include <group_of_pictures.h>

void video_sequence(){
	return;
};
int main(int argc,char* argv[]){
	if(argc != 2){
		puts("input file name !!");
		return 0;
	}
	FILE* mpeg_file = fopen(argv[1],"r");
	bit_buf* bb= new bit_buf(mpeg_file);
	sequence_header sh(bb);
	sh.read();
	group_of_pictures gop(bb);
	gop.read();

	printf("%d %d",sh.bb->file,bb->file);
	uint32_t tmp;
	tmp = bb->nextbits();
	printf("%x\n",tmp);
	tmp = sh.bb->nextbits();
	printf("%x\n",tmp);
	return 0;
}
