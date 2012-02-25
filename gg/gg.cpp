#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "gg.h"
#include "crc.h"
#include "arch_file.h"

#define READ(aaa) if(fread(&(aaa), sizeof(aaa), 1, f) != 1) { \
	fprintf(stderr, "%s:%d:read(%x:%d) failed\n", __FILE__, __LINE__, ftell(f),sizeof(aaa)); return false; }
#define SEEK(to) if(fseek(f, _POS=to, SEEK_SET) != 0) { \
	fprintf(stderr, "%s:%s:seek(%x) failed\n", to, __FILE__, __LINE__); return false; }

unsigned long _POS = 0;

/***/

bool gg_archives::load(char * filename) 
{
	f = fopen(filename, "rb");
	if(!f){
		perror("open");
		return false;
	}

	bool ret = load_header() && load_sections();

	fclose(f);
	return ret;
}

/***/

bool gg_archives::load_header()
{
	SEEK(0);
	READ(ah);
	
	if(ah.magic != ARCH_MAGIC){
		fprintf(stderr, "Bad magic: %08x, should be %08x\n", 
					ah.magic, ARCH_MAGIC);
		return false;
	}

	SEEK(0);
	CRC header_crc;
	
	for(int i=0;i<(int)ah.data_offset;i++){
		unsigned char byte = fgetc(f);
		if(i >= HEADER_CRC_OFFSET && i < HEADER_CRC_OFFSET + 4) byte = 0;
		header_crc.add(byte);
	}

	if(ah.header_crc != header_crc.get_value()) fprintf(stderr, 
		"warning: Bad header crc: %08x, should be %08x\n",
			ah.header_crc, header_crc.get_value());

	return true;
}

/***/

bool gg_archives::load_sections()
{
	int n_sections = ah.index_size / sizeof(struct arch_section_header);
	sections.resize(n_sections);
	
	SEEK(ah.index_offset);
	
	for(int i=0;i<n_sections;i++){
		READ(sections[i].ash);
		if((int)sections[i].ash.num != i){
			fprintf(stderr, 
				"Section number mismatch: %d, should be %d\n",
				sections[i].ash.num, i);
		}
	}

	bool ret = true;
	for(int i=0;i<n_sections;i++)
		if(!load_section(sections[i])) ret = false;
		
	return ret;
}

/***/

bool gg_archives::load_section(section & S)
{
	unsigned long block_off = S.ash.first_block;
	bool ret=true;

	if(S.ash.n_blocks > 100000) {
		fprintf(stderr, "Number of blocks too high: %d\n",
					S.ash.n_blocks);
		return false;
	}
	
	S.blocks.resize(S.ash.n_blocks);
	
	for(int i=0;i<(int)S.ash.n_blocks;i++){	
		SEEK(ah.data_offset + block_off);
		S.blocks[i].off = block_off;
		READ(S.blocks[i].asb);
		block_off = S.blocks[i].asb.next_block;
	}
	
	for(int i=0;i<(int)S.ash.n_blocks;i++)
		if(!load_section_data(S, S.blocks[i])) ret=false;

	return ret;
}

/***/

bool gg_archives::load_section_data(section & S, section::block & B)
{
	SEEK(ah.data_offset + B.off + 4);
		
	if(B.asb.length > 10000000) {
		fprintf(stderr, "[%08x] Block length too high: %d\n", 
				_POS, B.asb.length);
		return false;
	}

	CRC crc;
		
	for(int j=0;j<(int)B.asb.length - 4;j++) crc.add(fgetc(f));

	if(crc.get_value() != B.asb.crc){
		fprintf(stderr, "Warning: crc error in block %08x\n"
				"CRC: %08x, should be: %08x\n",
				B.off, B.asb.crc, crc.get_value());
	}
		
	if(S.ash.num != B.asb.section_num){
		fprintf(stderr, "Section number mismatch in block %08x\n"
				"Is: %d, should be %d\n",
				B.off, B.asb.section_num, S.ash.num);
	}

	if(B.asb.section_num == 0) return true;
	
	SEEK(ah.data_offset + B.off + sizeof(arch_section_block));

	if(B.asb.content_length > 10000000) {
		fprintf(stderr, "[%08x] Block content length too high: %d\n", 
			_POS, B.asb.content_length);
		return false;
	}

	vector<arch_msg_entry> ame;
	
	ame.resize(B.asb.content_length / sizeof(arch_msg_entry));
	
	for(int i=0;i<(int)ame.size();i++) READ(ame[i]);
	
	int M=0;
	for(vector<arch_msg_entry>::iterator it=ame.begin();
						it!=ame.end();++it,++M){
		unsigned char buf[10000];
		
		if(it->data_length > sizeof(buf)){
			fprintf(stderr, "[%08x] Message too long: %d\n",
				ah.data_offset + B.off 
				+ sizeof(arch_section_block)
				+ M * sizeof(arch_msg_entry),
				it->data_length);
			ignored_count++;
			continue;
		}
		
		SEEK(ah.data_offset + it->data_off);
		if(fread(buf, it->data_length, 1, f) != 1) {
			fprintf(stderr, "[%08x], Read error\n", _POS);
			return false;
		}

		switch(S.ash.num){
		case 1:
			if(!S.read_text(*it, buf, it->data_length))ignored_count++;
			break;
		case 2:
			if(!S.read_sms(*it, buf, it->data_length))ignored_count++;
			break;
		default:
			fprintf(stderr, "Warning: reading from section %d not implemented\n", S.ash.num);
			break;
		}
	}

	return true;
}

/***/

bool gg_archives::section::read_text(arch_msg_entry & ame, unsigned char * buf, unsigned int len)
{
	arch_msg_data & amd = *((arch_msg_data*)buf);

	if((amd.n_sent_to+5) * 4 > len){
		fprintf(stderr, "[%08x] Malformed message n_sent_to=%d\n", 
							_POS, amd.n_sent_to);
		return false;
	}

	if(amd.get_data_length() > 10000){
		fprintf(stderr, "[%08x] Invalid data_length: %lu\n",
			_POS, amd.get_data_length());
		return false;
	}
	
	message & M = *messages.insert(messages.end(), message());
	M.is_deleted = ame.deleted;
	M.from = amd.uin_from;
	M.sent = amd.sent_time;
	M.rcvd = amd.get_rcvd_time();
	M.was_sent = amd.n_sent_to > 0;
	
	for(int i=0;i<(int)amd.n_sent_to;i++){
		char buf[10];
		sprintf(buf, "%d", amd.get_recipient(i));
		M.to.push_back(buf);
	}

	unsigned char last_char = 0xff;
	unsigned char * data = amd.get_data();
	M.data.reserve(amd.get_data_length() - 1);
	
	for(int i=0;i<(int)amd.get_data_length() - 1;i++){
		M.data.push_back(data[i] ^ last_char);
		last_char = data[i];
	}

	return true;
}

/***/

bool gg_archives::section::read_sms(arch_msg_entry & ame, unsigned char * buf, unsigned int len)
{
	arch_sms_data & asd = *((arch_sms_data*)buf);

	if(!asd.verify(len)){
		fprintf(stderr, "[%08x] Malformed sms message\n", _POS);
		return false;
	}

	unsigned int dlen = asd.get_data_length();

	if(dlen > 10000){
		fprintf(stderr, "[%08x] Invalid data_length: %lu\n", _POS, dlen);
		return false;
	}
	
	message & M = *messages.insert(messages.end(), message());
	M.is_deleted = ame.deleted;
	M.from = 0;
	M.sent = asd.sent_time;
	M.rcvd = asd.sent_time;
	M.was_sent = true;
	M.to.push_back(asd.recipient);

	unsigned char last_char = 0xff;
	unsigned char * data = asd.get_data();
	M.data.reserve(dlen - 1);
	
	for(int i=0;i<(int)dlen;i++){
		M.data.push_back(data[i] ^ last_char);
		last_char = data[i];
	}

	return true;
}

/***/

int gg_archives::get_ignored_count()
{
	return ignored_count;
}

/***/
