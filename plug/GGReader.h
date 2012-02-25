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
#include "archreader.h"
#include "../gg/gg.h"

class GGReader :
	public ArchReader
{
public:
	GGReader(void);
	~GGReader(void);
	//przygotowuje archiwum do odczytu
	//sprawd� czego oczekuje konkretna implementacja!
	bool Prepare(void* params);
	//pobiera rekord z archiwum, zwraca false jesli archiwum jest juz puste
	bool getNext(void);
	//pobiera UID w�a�cieciela
	char* getOwnerUID(char* buff);
	// pobira czas odebrania aktualnej wiadomosci
	cTime64 getMsgTime(void);
	// nadawca aktualnej wiadomosci
	char* getMsgSenderUID(char* buff);
	//true dop�ki nie przejrzeli�my wszystkich
	bool msgHasReceivers();
	// odbiorcy wiadomo�ci (kolejni, jako osobne ci�gi)
	char* getMsgReceiverUID(char* buff);
	// tekst aktualnie prztwarzanej wiadomosci
	char* getMsgText(char* buff);
	// true je�li wiadomo�� wys�ana, false je�li odebrana
	bool getMsgWasSent(void);
	//ilo�� wiadomo�ci w archiwum, kt�re zosta�y zignorowane (b��dne itp)
	int getNumberOfIgnored();
protected: gg_archives arch;
		 vector<gg_archives::message>::iterator it;
		 bool hasUnreadReceivers;
		 list<string>::iterator lit;
};
