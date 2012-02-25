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

class ArchWriter
{
public:
	ArchWriter(void);
	~ArchWriter(void);
	//przygotowuje archiwum do zapisu
	//sprawd� czego oczekuje konkretna implementacja!
	virtual bool Prepare(void* params) = 0;
	//ustawia UID w�a�cieciela
	virtual void setOwnerUID(char* UID) = 0;
	// ustawia czas odebrania aktualnej wiadomosci
	virtual void setMsgTime(cTime64) = 0;
	// nadawca aktualnej wiadomosci
	virtual void setMsgSenderUID(char* UID) = 0;
	// odbiorca aktualnej wiadomosci
	virtual void setMsgReceiverUID(char* UID) = 0;
	// tekst aktualnie prztwarzanej wiadomosci
	virtual void setMsgText(char* text) = 0;
	// true je�li wiadomo�� wys�ana, false je�li odebrana
	virtual void setMsgWasSent(bool wasSent) = 0;
	//dodaje aktualn� wiadomo��
	//zwraca true je�li wszystko posz�o dobrze
	//za�o�enie wiadomo�ci s� dodawane wg. kolejno�ci czasowej
	virtual bool addMsg(void) = 0;
};
