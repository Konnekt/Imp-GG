#include <stdio.h>
#include "crc.h"

struct gg_arch_header {
	unsigned long magic;
	unsigned long field_4;
	unsigned long index_offset;
	unsigned long index_size;
	unsigned long file_size;
	unsigned long data_offset;
	unsigned long field_18;
	unsigned long field_1C;
	unsigned long field_20;
	unsigned long user_id;
	unsigned long header_crc;
};

struct gg_section_entry {
	unsigned long section_num;
	unsigned long section_parts;
	unsigned long section_start; // offset from data_offset
	unsigned long field_C;
	unsigned long field_10;
};

struct gg_section_list_element {
	unsigned long crc;
	unsigned long section_num;
	unsigned long length;
	unsigned long next_element_start;
	unsigned long content_length;
};

struct gg_data_header1 {
	unsigned long deleted;
	unsigned long off;
	unsigned long length;
	unsigned long block_start;
};

struct gg_message_header {
	unsigned long time;
	unsigned long uin_from;
	unsigned long sent_to;
	unsigned long uins[1];
};

void check_header_crc(FILE * f, struct gg_arch_header * gah)
{
unsigned long crc;
int i;
int ignore = (int)(&((struct gg_arch_header*)NULL)->header_crc);
fseek(f, 0, SEEK_SET);
CRC_START(crc);
	for(i=0;i<gah->data_offset;i++){
	unsigned char c = fgetc(f);
		if(i >= ignore && i < ignore+4) c=0;
	CRC_NEXT(crc, c);
	}
crc = CRC_VALUE(crc);

	if(crc != gah->header_crc)
	printf("crc error: %08x vs %08x\n", crc, gah->header_crc);
	
}

char * decode_message(unsigned char * msg, int len)
{
unsigned char last = ~0;
int i;
	for(i=0;i<len;i++){
	unsigned char newc = msg[i] ^ last;
	last = msg[i];
	msg[i] = newc;
	}

return (char*)msg;
}

void read_message(FILE * f, struct gg_arch_header * gah, struct gg_data_header1 * gdh1)
{
union {
	unsigned char buf[2000];
	struct gg_message_header gmh;
} u;
unsigned long length;

	if(gdh1->length > sizeof(u.buf)) return;

fseek(f, gah->data_offset + gdh1->off, SEEK_SET);
fread(u.buf, gdh1->length, 1, f);
	if(gdh1->deleted) printf("(deleted) ");
	if(u.gmh.sent_to == 0) printf("Received from %d", u.gmh.uin_from);
	else{
	int i;
	printf("Sent to");
		for(i=0;i<u.gmh.sent_to;i++) printf(" %d", u.gmh.uins[i]);
	}
	
printf(" [%d :: %d]\n", u.gmh.time, u.gmh.uins[u.gmh.sent_to]);
length = u.gmh.uins[u.gmh.sent_to+1];

printf("%s\n", decode_message((unsigned char*)&u.gmh.uins[u.gmh.sent_to+2], length));
}

void print_section(FILE * f, struct gg_arch_header * gah, struct gg_section_entry * gse)
{
int i,j;
unsigned long off = gse->section_start;

printf("Section %d: %08x [%d] [%08x %08x]\n", 
	gse->section_num, gse->section_start, gse->section_parts,
	gse->field_C, gse->field_10
	);


	for(i=0;i<gse->section_parts;i++){
	struct gg_section_list_element gsle;
	unsigned long crc;
	fseek(f, gah->data_offset + off, SEEK_SET);
	fread(&gsle, sizeof(gsle), 1, f);
	
	fseek(f, gah->data_offset + off + 4, SEEK_SET);
	
	CRC_START(crc);
		for(j=0;j<gsle.length-4;j++) CRC_NEXT(crc, fgetc(f));
	crc = CRC_VALUE(crc);

	printf("GSLE: sn=%d length=%08x off=%08x l=%d\n", gsle.section_num,
		gsle.length, gsle.content_length/16);

		if(crc != gsle.crc)
		printf("crc error: %08x vs %08x\n", crc, gsle.crc);

		if(gsle.section_num != 0)
		for(j=0;j<gsle.content_length;j+=sizeof(struct gg_data_header1)){
		struct gg_data_header1 gdh1;
		fseek(f, gah->data_offset + off + sizeof(gsle) + j,
								SEEK_SET);
		fread(&gdh1, sizeof(gdh1), 1, f);
		read_message(f, gah, &gdh1);
		}
	
	off = gsle.next_element_start;
	}
}

void print_sections(FILE * f, struct gg_arch_header * gah)
{
int ns = gah->index_size / sizeof(struct gg_section_entry);
int i, j;
	for(i=0;i<ns;i++){
	struct gg_section_entry gse;
	
	fseek(f, gah->index_offset + i * sizeof(gse), SEEK_SET);
	fread(&gse, sizeof(gse), 1, f);
	
	print_section(f, gah, &gse);
	}
}

int main(int ac, char ** av)
{
struct gg_arch_header gah;
FILE * input;
crc_init();
input = fopen(av[1], "r");
fread(&gah, sizeof(gah), 1, input);
printf("%08x %08x %08x %08x\n",
	gah.field_4, gah.field_18, gah.field_1C, gah.field_20);
check_header_crc(input, &gah);
printf("uid: %d\n", gah.user_id ^ 0xFFFFFD66);
print_sections(input, &gah);
}
