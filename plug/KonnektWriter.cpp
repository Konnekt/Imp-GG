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

#include ".\konnektwriter.h"

int stringInArray(char array[15][65000], char* string)
{
	int result = -1;
	for(int i=0;i<15;i++)
	{
		if(strcmp(array[i],string)==0)result=i;
		if(result>=0)break;
	}
	return result;
}

int getCntByDisplayOrPhone(const char* string)
{
	//blokujemy liste kontaktów
	Ctrl->DTlock(DTCNT,-1);
	for (int i = 1; i < ICMessage(IMC_CNT_COUNT); i++) {
		std::string display = GETCNTC(i, CNT_DISPLAY);
		if(strcmp(string,display.c_str())==0)
		{
			//odblokowujemy listê kontaktów
			Ctrl->DTunlock(DTCNT,-1);
			return i;
		}
		std::string phone = GETCNTC(i, CNT_CELLPHONE);
		if(strcmp(string,phone.c_str())==0)
		{
			//odblokowujemy listê kontaktów
			Ctrl->DTunlock(DTCNT,-1);
			return i;
		}
	}
	//odblokowujemy listê kontaktów
	Ctrl->DTunlock(DTCNT,-1);
	return 0;
}

KonnektWriter::KonnektWriter(void)
{
}

KonnektWriter::~KonnektWriter(void)
{
}

//przygotowuje archiwum do zapisu
//oczekuje inta (NET) identyfikuj¹cego sieæ
bool KonnektWriter::Prepare(void* params)
{
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::Prepare.");
#endif
	msgReceiversCount = 0;
	currTalker=0;
	counter=0;
	netID = *((int*)params);
	//jeœli nie jestesmy pod³¹czeniu do UI
	if(!Ctrl)return false;
	return true;
}
//ustawia UID w³aœcieciela
void KonnektWriter::setOwnerUID(char* UID)
{
	sprintf(ownerUID,"%s",UID);
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::SetOwnerUID");
#endif
}
// ustawia czas odebrania aktualnej wiadomosci
void KonnektWriter::setMsgTime(cTime64 time)
{
	_time=time;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::setMsgTime");
#endif
}
// nadawca aktualnej wiadomosci
void KonnektWriter::setMsgSenderUID(char* UID)
{
	sprintf(fromUID,"%s",UID);
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::SetMsgSenderUID");
#endif
}
// odbiorca aktualnej wiadomosci
void KonnektWriter::setMsgReceiverUID(char* UID)
{
	msgReceiversCount++;
	//nadpisujemy poprzedniego odbiorcê, to nowa wiadomoœæ
	if(msgReceiversCount==1)
	{
		sprintf(toUID,"%s",UID);
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::SetMsgReceiverUID");
#endif
	}
	else //dopisujemy odbiorcê do konferencji
	{
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::SetMsgReceiverUID dopisujê kolejnego konnferencjowicza");
#endif
		sprintf(toUID,"%s;%s",toUID,UID);
	}
}
// tekst aktualnie prztwarzanej wiadomosci
void KonnektWriter::setMsgText(char* text)
{
	sprintf(msgText,"%s",text);
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::SetMsgText");
#endif
}
// true jeœli wiadomoœæ wys³ana, false jeœli odebrana
void KonnektWriter::setMsgWasSent(bool was)
{
	wasSent = was;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::MsgWasSent");
#endif
}
//dodaje aktualn¹ wiadomoœæ
//zwraca true jeœli wszystko posz³o dobrze
bool KonnektWriter::addMsg(void)
{
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Rozpoczynam Writer::addMsg");
#endif
	//czekamy, a¿ ¿adne wiadomoœci nie czekaj¹ w kolejce do przetworzenia
	//while(IMessage(IMC_MESSAGEWAITING)>0)SleepEx(50,true);
	//to wysypuje Konnekta
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg IMC_MESSAGEWAITING");
#endif
	counter++;
	if(counter>=1000)
	{
		counter=0;
		SleepEx(1000,true);
	}
	else SleepEx(0,true);
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg Sleep if za nami");
#endif
	char currTalkerChar[65000];
	cTime64 currTalkerTime;
	cMessage msg;
	msg.net = netID;	
	msg.type = MT_MESSAGE;
	sHISTORYADD hist;                   
	hist.cnt = 0;
	hist.dir = "messages";
	hist.m = &msg; //struktura cMessage
	hist.s_size = sizeof(sHISTORYADD);
	msg.time = _time;
	currTalkerTime = msg.time;
	char fromUIDbuff[65000];
	char nameBuff[65000];
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg deklaracja zmiennych");
#endif
	sprintf(fromUIDbuff,"%s",fromUID);
	msg.fromUid = fromUIDbuff;//UID
	char toUIDbuff[65000];
	string ext; //jak bêdzie wewn¹trz {}, to za nim ju¿ nie istnieje!
	string displayName;
	if(msgReceiversCount==1)sprintf(toUIDbuff,"%s",toUID);

	//-------------Konnfer----------------------------------------
	else 
	{// ustawiamy nazwê mówi¹cego w Konnferencji
		int cnt = ICMessage(IMC_FINDCONTACT , netID , (int)msg.fromUid);
		if(cnt != -1)displayName = GETCNTC(cnt , CNT_DISPLAY);
		else displayName=msg.fromUid;
		msg.net = konnfer::net;
		ext  = SetExtParam(msg.ext , MEX_DISPLAY , displayName);
		msg.ext = (char*) ext.c_str();
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg Konferencyjna, ustawi³em display");
#endif
	}
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg wype³nienie UIDów ju¿ za nami");
#endif
	if(msgReceiversCount>1)
	{
		//wiadomoœæ konferencyjna wys³ana
	if(wasSent)
		{
			cleanupUIDs(toUID,ownerUID);
			sprintf(toUIDbuff,"%s@%d",toUID,netID);
			msg.toUid = toUIDbuff;
			sprintf(currTalkerChar,"%s",msg.toUid);
			hist.name = msg.toUid;//nazwa pliku (cnt==0)
			hist.cnt = ICMessage(IMC_FINDCONTACT,konnfer::net, (int)msg.toUid);// IMC_CNT_FIND czy cos takiego, po wartosci net i uidzie
			msg.flag = MF_SEND|MF_AUTOMATED;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg konferencja if(wasSent)==true\nTo %s\nFrom %s",msg.toUid,msg.fromUid);
#endif
		}
		//wiadomoœæ konferencyjna odebrana
		else
		{
			cleanupUIDs(toUID,ownerUID);

			sprintf(toUIDbuff,"%s@%d",toUID,netID);
			msg.toUid = toUIDbuff;
			sprintf(nameBuff,"%s;%s",msg.fromUid,toUID);
			cleanupUIDs(nameBuff,ownerUID);

			sprintf(nameBuff,"%s@%d",nameBuff,netID);
			sprintf(currTalkerChar,"%s",nameBuff);
			hist.name = nameBuff;
			hist.cnt = ICMessage(IMC_FINDCONTACT,konnfer::net, (int)nameBuff);// IMC_CNT_FIND czy cos takiego, po wartosci net i uidzie
			msg.flag = MF_AUTOMATED;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg konferencja if(wasSent)==false\nTo %s\nFrom %s",msg.toUid,msg.fromUid);
#endif
		}
	}

	//-------------Konnfer-------------------------------------------
	//---------------SMS---------------------------------------------
	else if(netID==NET_SMS)
	{
		hist.dir = "sms";
		msg.toUid = toUIDbuff;
		if(wasSent)hist.name = msg.toUid;//nazwa pliku (cnt==0)
		else hist.name = msg.fromUid;
		//problem w tym, ¿e w archiwum gadu jest tekst, a nie numer...
		hist.cnt = getCntByDisplayOrPhone(hist.name);
		sprintf(currTalkerChar,"%s",msg.toUid);
		msg.flag = MF_AUTOMATED|MF_HTML;
		msg.type = MT_SMS;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg %s",msgText);
	IMLOG("SKOLIMA_ Writer::addMsg SMS");
#endif
	}
	//---------------SMS---------------------------------------------
	//wiadomoœæ zwyk³a wys³ana
	else if(wasSent)
	{
		msg.toUid = toUIDbuff;
		hist.name = msg.toUid;//nazwa pliku (cnt==0)
		hist.cnt = ICMessage(IMC_FINDCONTACT,netID, (int)msg.toUid);// IMC_CNT_FIND czy cos takiego, po wartosci net (gg=10) i uidzie
		sprintf(currTalkerChar,"%s",msg.toUid);
		msg.flag = MF_SEND|MF_AUTOMATED;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg if(wasSent)==true");
#endif
	}
	//wiadomoœæ zwyk³a odebrana
	else
	{
		msg.toUid = toUIDbuff;
		sprintf(currTalkerChar,"%s",msg.fromUid);
		hist.name = msg.fromUid;
		hist.cnt = ICMessage(IMC_FINDCONTACT,netID, (int)msg.fromUid);// IMC_CNT_FIND czy cos takiego, po wartosci net (gg=10) i uidzie
		msg.flag = MF_AUTOMATED;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg if(wasSent)==false");
#endif
	}
	int talkersArrayPtr = stringInArray(lastTalkerChar,currTalkerChar);
	cTime64 timeDiff;
	if(talkersArrayPtr>=0)timeDiff.sec = max((currTalkerTime.sec-lastTalkerTime[talkersArrayPtr].sec)+1,0);
	else timeDiff.sec = MAXTIMESPAN;
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg zaraz bêdzie kombinowanie z sesj¹...");
#endif
	if(timeDiff.sec<MAXTIMESPAN)hist.session=1;// (albo 0) 1 to kontynuacja rozmowy, 0 to tworzenie nowej 
	else 
	{
		hist.session = 0;
		sprintf(lastTalkerChar[currTalker],"%s",currTalkerChar);
		if(talkersArrayPtr<0)talkersArrayPtr = currTalker;
		currTalker++;
		currTalker%=15;
	}
	if(talkersArrayPtr>=0)
	{
		lastTalkerTime[talkersArrayPtr]=currTalkerTime;
	}
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg wrzucamy text do wiadomoœci");
#endif
	char msgTextbuff[65000];
	sprintf(msgTextbuff,"%s",msgText);
	msg.body = msgTextbuff;
	msg.s_size = sizeof(cMessage);
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Writer::addMsg Próbujê dodaæ wiadomoœæ");
#endif
	ICMessage(IMI_HISTORY_ADD,(int)&hist);
#ifdef DEBUG_HI
	IMLOG("SKOLIMA_ Dodano wiadomoœæ.");
#endif
	msgReceiversCount = 0;
	return true;
}
