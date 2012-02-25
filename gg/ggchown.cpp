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

int main(int ac, char ** av)
{
	if(!av[1]){
		fprintf(stderr, "Usage: %s filename [new uin]\n", av[0]);
		return 1;
	}
	FILE * f = fopen(av[1], "r+b");
	struct arch_header ah;
	fread(&ah, sizeof(ah), 1, f);
	printf("Current uin: %d\n", ah.get_uin());

	if(!av[2]) return 0;
	ah.set_uin(atoi(av[2]));
	printf("New uin: %d\n", ah.get_uin());
	ah.header_crc = 0;
	fseek(f, 0, SEEK_SET);
	fwrite(&ah, sizeof(ah), 1, f);
	fseek(f, 0, SEEK_SET);

	CRC crc;
	for(int i=0;i<(int)ah.data_offset;i++)
		crc.add(fgetc(f));
	ah.header_crc = crc.get_value();

	fseek(f, 0, SEEK_SET);
	fwrite(&ah, sizeof(ah), 1, f);
	fclose(f);
}
