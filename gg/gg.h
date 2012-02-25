#ifndef _GG_H_
#define _GG_H_ 1

#include <stdio.h>
#include <time.h>
#include "arch_file.h"

#include <list>
#include <string>
#include <vector>

using std::list;
using std::string;
using std::vector;

class gg_archives {
public:
	struct message {
		bool is_deleted, was_sent;
		time_t sent, rcvd;
		int from;
		list<string> to;
		string data;
	};
	
	int ignored_count;
	class section {
	friend class gg_archives;
	private:
		struct arch_section_header ash;
		struct block {
			unsigned long off;
			arch_section_block asb;
		};
		
		vector<block> blocks;
		bool read_text(arch_msg_entry & ame, unsigned char * buf, unsigned int len);
		bool read_sms(arch_msg_entry & ame, unsigned char * buf, unsigned int len);
	public:
		vector<message> messages;
	};

	vector<section> sections;

	bool load(char * file);
	void clear() { sections.clear(); }

	int get_uin() { return ah.get_uin(); }
	int get_ignored_count();
	
private:
	FILE * f;
	struct arch_header ah;
	bool load_header();
	bool load_sections();
	bool load_section(section & S);
	bool load_section_data(section & S, section::block & B);
};
#endif
