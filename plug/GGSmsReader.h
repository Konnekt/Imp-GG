/*
written for Imp-GG project
Copyright (C) 2005 Skolima (skolima.prv.pl)

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

#pragma once
#include "ggreader.h"
#include "SkolimaUtilz.h"

class GGSmsReader :
	public GGReader
{
public:
	GGSmsReader(void);
	~GGSmsReader(void);
	//przygotowuje archiwum do odczytu
	//sprawdŸ czego oczekuje konkretna implementacja!
	bool Prepare(void* params);
	//pobiera rekord z archiwum, zwraca false jesli archiwum jest juz puste
	bool getNext(void);
	// true jeœli wiadomoœæ wys³ana, false jeœli odebrana
	bool getMsgWasSent(void);
	// tekst aktualnie prztwarzanej wiadomosci
	char* getMsgText(char* buff);
};
