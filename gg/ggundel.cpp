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

#include <stdio.h>
#include <stdlib.h>
#include "arch_file.h"
#include "crc.h"

#define FSEEK(to,whence) if(fseek(f, to, whence) != 0) { \
	fprintf(stderr, "Seek(%d) failed\n", to); return 1; }

#define SEEK(to) FSEEK(to,SEEK_SET)
	
#define READ(aaa) if(fread(&(aaa), sizeof(aaa), 1, f) != 1) { \
	fprintf(stderr, "Read failed\n"); return 1; }

#define WRITE(aaa) if(fwrite(&(aaa), sizeof(aaa), 1, f) != 1) { \
	fprintf(stderr, "Write failed\n"); return 1; }


int main(int ac, char ** av)
{
	if(!av[1]){
		fprintf(stderr, "Usage: %s filename\n", av[0]);
		return 1;
	}
	
	FILE * f = fopen(av[1], "r+b");
	struct arch_header ah;
	READ(ah);

	for(int i=1;i<(int)(ah.index_size / sizeof(struct arch_section_header));i++){
		struct arch_section_header ash;
		printf("Section %d...", i);
		fflush(stdout);
		
		SEEK(ah.index_offset + i * sizeof(struct arch_section_header));
		READ(ash);

		unsigned long block_off = ash.first_block;
		int undeleted=0;

		for(int j=0;j<(int)ash.n_blocks;j++){
			struct arch_section_block asb;
			bool block_modified = false;
			
			SEEK(ah.data_offset + block_off);
			READ(asb);
		
			for(int k=0;k<(int)(asb.content_length / sizeof(arch_msg_entry));k++) {
				struct arch_msg_entry ame;
				READ(ame);
				if(ame.deleted != 0){
					if(ame.deleted == 1){
						FSEEK(-sizeof(arch_msg_entry), SEEK_CUR);
						ame.deleted = 0;
						WRITE(ame);
						
						++undeleted;
						block_modified = true;
					}else{
						fprintf(stderr, "Warning: unexpected 'flags' value: %d\n", ame.deleted);
					}
				}
			}

			if(block_modified) {
				SEEK(ah.data_offset + block_off + 4);
				CRC crc;
				for(int j=0;j<(int)asb.length - 4;j++) crc.add(fgetc(f));
				asb.crc = crc.get_value();
				SEEK(ah.data_offset + block_off);
				WRITE(asb);
			}
			block_off = asb.next_block;
		}

		printf("%d messages undeleted\n", undeleted);
	}

	fclose(f);
}
