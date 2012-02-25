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
#include "gg.h"


int main(int ac, char ** av)
{
	gg_archives arch;
	if(!av[1]){
		fprintf(stderr, "Usage: %s filename\n", av[0]);
		return 1;
	}

	if(!arch.load(av[1])) return 1;

	if(arch.sections.size() >= 2){
		vector<gg_archives::message>::iterator it;
		for(it=arch.sections[1].messages.begin();
			it!=arch.sections[1].messages.end();
			it++){

			if(it->is_deleted) continue;
			
			if(it->was_sent) printf("chatsend,%s,,%d", 
				it->to.begin()->c_str(), it->sent);
			else printf("chatrecv,%d,,%d,%d",
				it->from, it->sent, it->rcvd);
				
			printf(",\"");

			for(int i=0;i<(int)it->data.size();i++){
				unsigned char c = it->data.data()[i];
				switch(c){
					case 0: break;
					case '\r': printf("\\r"); break;
					case '\n': printf("\\n"); break;
					case '\t': printf("\\t"); break;
					case '\\': printf("\\\\"); break;
					default:
					if(c < ' ') printf("\\x%02x", c);
					else printf("%c", c);
				}
			}

			printf("\"\n");
		}
	}
}
