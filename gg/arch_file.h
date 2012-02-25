/*
original author anszom (http://v-lo.krakow.pl/~anszom/index.pl.shtml)

modified for Imp-GG project
Copyright (C) 2005 Skolima (http://skolima.prv.pl)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef ARCH_FILE_H
#define ARCH_FILE_H

#include <string.h>

#define ARCH_MAGIC 	(('R'<<0)|('C'<<8)|('0'<<16)|('3'<<24))

struct arch_header {
	unsigned long magic;
	unsigned long __field__4;
	unsigned long index_offset;
	unsigned long index_size;
	unsigned long file_size;
	unsigned long data_offset;
	unsigned long __field__18;
	unsigned long __field__1C;
	unsigned long __field__20;
	unsigned long user_id;
	unsigned long header_crc;

#define HEADER_CRC_OFFSET ((int)(&((arch_header*)0)->header_crc))

	unsigned long get_uin() { return user_id ^ 0xFFFFFD66; }
	void set_uin(unsigned long uin) { user_id = uin ^ 0xFFFFFD66; }
};

struct arch_section_header {
	unsigned long num;
	unsigned long n_blocks;
	unsigned long first_block; // offset from data_offset
	unsigned long __field__C;
	unsigned long __field__10;
};

struct arch_section_block {
	unsigned long crc;
	unsigned long section_num;
	unsigned long length;		// in bytes
	unsigned long next_block;	// offset from data_offset
	unsigned long content_length;	// number of messages * sizeof(arch_msg_entry)
};

struct arch_msg_entry {
	unsigned long deleted;
	unsigned long data_off;
	unsigned long data_length;
	unsigned long my_block_start;
};

struct arch_msg_data {
	unsigned long sent_time;
	unsigned long uin_from;
	unsigned long n_sent_to;
	/*
	 * incoming message:
	 * 	unsigned long rcvd_time;
	 * 	unsigned long length;
	 * 	char data[length];
	 * 	
	 * outgoing message:
	 * 	unsigned long uins[n_sent_to];
	 * 	unsigned long rcvd_time; // ??
	 * 	unsigned long length;
	 * 	char data[length];
	 */
	unsigned long a[1];

	bool was_sent() { return n_sent_to > 0; }
	unsigned long get_recipient(int n) { return a[n]; }
	unsigned long get_rcvd_time() { return a[n_sent_to]; }
	unsigned long get_data_length() { return a[n_sent_to+1]; }
	unsigned char * get_data() { return (unsigned char*)&a[n_sent_to+2]; }
};

struct arch_sms_data {
	unsigned long sent_time;
	char recipient[1];
	/*
	 * 	unsigned long length;
	 * 	char data[length];
	 */
	
	unsigned long get_data_length() { return *(unsigned long*)(recipient+strlen(recipient)+1); }
	unsigned char * get_data() { return (unsigned char*)(recipient+strlen(recipient)+5); }
	bool verify(int len) { for(int i=0;i<len-8;i++) if(recipient[i] == 0) return true; return false; }
};


#endif
