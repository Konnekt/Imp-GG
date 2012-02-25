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
#include <windows.h>
#include <string>

using namespace std;

#include "archwriter.h"
#include "../headers/c++/konnekt/plug_export.h"
#include "../headers/c++/konnekt/plug.h"
#include "../headers/c++/konnekt/ui_message_controls.h"
#include "../headers/c++/konnekt/ui.h"
#include "../headers/c++/konnekt/KONNferencja.h"
#include "../headers/c++/konnekt/lib/time64.h"
#include "../headers/c++/konnekt/plug_func.h"
#include "skolimaUtilz.h"

//maksymalna iloœæ sekund, o któr¹ mog¹ byæ od siebie oddalone kolejne wiadomoœci z rozmowy
#define MAXTIMESPAN 60*60*3

class KonnektWriter :
	public ArchWriter
{
public:
	KonnektWriter(void);
	~KonnektWriter(void);
	//przygotowuje archiwum do zapisu
	bool Prepare(void* params) ;
	//ustawia UID w³aœcieciela
	void setOwnerUID(char* UID) ;
	// ustawia czas odebrania aktualnej wiadomosci
	void setMsgTime(cTime64 time) ;
	// nadawca aktualnej wiadomosci
	void setMsgSenderUID(char* UID) ;
	// odbiorca aktualnej wiadomosci
	void setMsgReceiverUID(char* UID) ;
	// tekst aktualnie prztwarzanej wiadomosci
	void setMsgText(char* text) ;
	// true jeœli wiadomoœæ wys³ana, false jeœli odebrana
	void setMsgWasSent(bool was) ;
	//dodaje aktualn¹ wiadomoœæ
	//zwraca true jeœli wszystko posz³o dobrze
	bool addMsg(void) ;
private :
	char ownerUID[65000];
	char lastTalkerChar[15][65000];
	cTime64 lastTalkerTime[15];		
	int currTalker;
	__int64 _time;
	char fromUID[65000];
	char toUID[65000];
	bool wasSent;
	char msgText[65000];
	int netID;
	int counter;
	int msgReceiversCount;
};
