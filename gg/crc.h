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

class CRC {
private:
	unsigned long value;
	static unsigned long table[256];
	static bool table_initialized;
	static void init_table();
	
public:
	CRC(): value(~0) { if(!table_initialized) init_table(); }
	inline void add(unsigned char byte) {
		value=((value>>8) ^ table[(value ^ byte) & 0xFF]); }
	inline unsigned long get_value() { return ~value; }
};
