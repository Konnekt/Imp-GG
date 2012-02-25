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

#include ".\ggsmsreader.h"

GGSmsReader::GGSmsReader(void)
{
}

GGSmsReader::~GGSmsReader(void)
{
}

	//przygotowuje archiwum do odczytu, np. pokazuje okno OpenFile
	//sprawdŸ czego oczekuje konkretna implementacja!
bool GGSmsReader::Prepare(void* params)
{
	arch.ignored_count = 0;
	char* fileName = *((char**)params);
	if(!arch.load(fileName))//"archives.dat")) 
	{	
		return false;
	}
	//
	it=(arch.sections[2].messages.begin()-1);//we load the messages, [2] contains sms messages
	hasUnreadReceivers = true;
	return true;
}

//pobiera rekord z archiwum, zwraca false jesli archiwum jest juz puste
bool GGSmsReader::getNext(void)
{
	hasUnreadReceivers = true;
	if(arch.sections.size() < 2||it>=arch.sections[2].messages.end()) return false;
	it++;
	while(it->is_deleted&&it<arch.sections[2].messages.end())it++;
	if(it>=arch.sections[2].messages.end()) return false;
	lit=it->to.begin();
	return true;
}

// true jeœli wiadomoœæ wys³¹na, false jeœli odebrana
bool GGSmsReader::getMsgWasSent(void)
{
	//czy GG pozwala w ogóle odbieraæ SMSy?
	bool result = true;
	return result;
}

// tekst aktualnie prztwarzanej wiadomosci
char* GGSmsReader::getMsgText(char* buff)
{
	sprintf(buff,"%s",it->data.c_str());
	char * oldBuff = buff;
	//kiev siê na tym sypa³...
	buff = StringReplace(buff,"\n"," ");
	delete oldBuff;
	return buff;
}
