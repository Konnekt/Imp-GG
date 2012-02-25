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

#include ".\ggreader.h"


GGReader::GGReader(void)
{
}

GGReader::~GGReader(void)
{
}
	//przygotowuje archiwum do odczytu, np. pokazuje okno OpenFile
	//sprawdü czego oczekuje konkretna implementacja!
bool GGReader::Prepare(void* params)
{
	arch.ignored_count = 0;
	char* fileName = *((char**)params);
	if(!arch.load(fileName))//"archives.dat")) 
	{	
		return false;
	}
	//
	it=(arch.sections[1].messages.begin()-1);//we load the messages, [2] contains sms messages
	hasUnreadReceivers = true;
	return true;
}

	//pobiera rekord z archiwum, zwraca false jesli archiwum jest juz puste
bool GGReader::getNext(void)
{
	hasUnreadReceivers = true;
	if(arch.sections.size() < 2||it>=arch.sections[1].messages.end()) return false;
	it++;
	while(it->is_deleted&&it<arch.sections[1].messages.end())it++;
	if(it>=arch.sections[1].messages.end()) return false;
	lit=it->to.begin();
	return true;
}

	//pobiera UID w≥aúcieciela
char* GGReader::getOwnerUID(char* buff)
{
	sprintf(buff,"%d",arch.get_uin());
	return buff;
}

	// pobiera czas odebrania aktualnej wiadomosci
cTime64 GGReader::getMsgTime(void)
{
	cTime64 * myTime = new cTime64(it->rcvd);
	return *myTime;
}

	// nadawca aktualnej wiadomosci
char* GGReader::getMsgSenderUID(char* buff)
{
	//czasem siÍ zdarza, øe was_sent ustawione jest odwrotnie
	if(it->was_sent&&(it->from==arch.get_uin())){ 
		sprintf(buff,"%d",arch.get_uin());
	}else{
		sprintf(buff,"%d",it->from);
	}
	return buff;
}


	//true dopÛki nie przejrzeliúmy wszystkich
bool GGReader::msgHasReceivers()
{
	return hasUnreadReceivers;
}

	// odbiorca aktualnej wiadomosci
char* GGReader::getMsgReceiverUID(char* buff)
{
	//czasem siÍ zdarza, øe was_sent ustawione jest odwrotnie
	if(!it->was_sent&&(it->from!=arch.get_uin())){ 
		sprintf(buff,"%d",arch.get_uin());
	}else{
		sprintf(buff,"%s",lit->c_str());
		++lit;
	}
	if(lit==it->to.end())hasUnreadReceivers = false;
	return buff;
}

	// tekst aktualnie prztwarzanej wiadomosci
char* GGReader::getMsgText(char* buff)
{
	sprintf(buff,"%s",it->data.c_str());
	return buff;
}

// true jeúli wiadomoúÊ wys≥πna, false jeúli odebrana
bool GGReader::getMsgWasSent(void)
{
	//czasem siÍ zdarza, øe was_sent ustawione jest odwrotnie
	bool result = true;
	if(arch.get_uin()!=it->from)result = false;
	return result;
}

int GGReader::getNumberOfIgnored()
{
	return arch.get_ignored_count();
}
