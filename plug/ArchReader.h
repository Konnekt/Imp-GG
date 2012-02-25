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
#include "../headers/c++/konnekt/lib/time64.h"

class ArchReader
{
public:
	ArchReader(void);
	~ArchReader(void);
	//przygotowuje archiwum do odczytu
	//sprawdŸ czego oczekuje konkretna implementacja!
	virtual bool Prepare(void* params) = 0;
	//pobiera rekord z archiwum, zwraca false jesli archiwum jest juz puste
	virtual bool getNext(void) = 0;
	//pobiera UID w³aœcieciela
	virtual char* getOwnerUID(char* buff) = 0;
	// pobira czas odebrania aktualnej wiadomosci
	virtual cTime64 getMsgTime(void) = 0;
	// nadawca aktualnej wiadomosci
	virtual char* getMsgSenderUID(char* buff) = 0;
	//true dopóki nie przejrzeliœmy wszystkich
	virtual bool msgHasReceivers() = 0;
	// odbiorcy wiadomoœci (kolejni, jako osobne ci¹gi)
	virtual char* getMsgReceiverUID(char* buff) = 0;
	// tekst aktualnie prztwarzanej wiadomosci
	virtual char* getMsgText(char* buff) = 0;
	// true jeœli wiadomoœæ wys³¹na, false jeœli odebrana
	virtual bool getMsgWasSent(void) = 0;
	//iloœæ wiadomoœci w archiwum, które zosta³y zignorowane(b³êdne itp)
	virtual int getNumberOfIgnored() = 0;
};
