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
#include <time.h>
#include "gg.h"

bool undel = false;
gg_archives arch;

void show_section(int sect)
{
	vector<gg_archives::message>::iterator it;
	for(it=arch.sections[sect].messages.begin();
		it!=arch.sections[sect].messages.end();
		it++){
		
		if(it->is_deleted && !undel) continue;
		char * tm = asctime(localtime(&it->rcvd));
		
		if(it->was_sent){ 
			printf(">>   to: ");
			bool first=true;
			
			for(list<string>::iterator lit=it->to.begin();
				lit!=it->to.end();++lit){
				if(!first) printf(", ");
				printf("%s", lit->c_str());
				first=false;
			}

			printf(" at %s", tm);
		}else{
			printf("<< from: %7d at %s", it->from, tm);
		}
		puts(it->data.c_str());
	}
}

int main(int ac, char ** av)
{
	if(av[1] && !strcmp(av[1], "-u")){
		undel=true;
		++av;
	}
	
	if(!av[1]){
		fprintf(stderr, "Usage: %s [-u] filename [section]\n", av[0]);
		return 1;
	}

	if(!arch.load(av[1])) return 1;
	
	unsigned int sect = 0;
	if(av[2]) sect = atoi(av[2]);

	printf("Archives for UIN: %d\n", arch.get_uin());

	if(sect > 0) show_section(sect);
	else for(unsigned int i=1;i<arch.sections.size();i++) show_section(i);
}
