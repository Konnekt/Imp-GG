/* 
   Powinna zostaæ skompilowana jako Win32DLL z wykorzystaniem
   wielow¹tkowej wersji biblioteki CRT!

   (c)2002 hao | Stamina
   http://www.stamina.eu.org/konnekt/sdk/
   (template kodu wtyczki)
*/ 

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

/*
PLUG Imp-GG by Skolima
http://skolima.prv.pl

*/

#include <windows.h>
#include "../headers/c++/konnekt/plug_export.h"
#include "../headers/c++/konnekt/plug.h"
#include "../headers/c++/konnekt/ui_message_controls.h"
#include "../headers/c++/konnekt/lib/time64.h"

#include "../plug/GGReader.h"
#include "../plug/GGSmsReader.h"
#include "../plug/KonnektWriter.h"
#include "../plug/ArchImporter.h"

// Wartosc net tej wtyczki - no idea, chyba u¿ywaæ tego nie bêdê...
#define NET_IMPGG 551 
#define myPLUGINNAME "Imp-GG"
#define myPLUGVERSION "0.0.2.3"
int __stdcall DllMain(void * hinstDLL, unsigned long fdwReason, void * lpvReserved)
{
        return true;
}
//---------------------------------------------------------------------------

int Run()
{
  if(MessageBox(NULL,"Czy doda³eœ ju¿ swoich znajomych do listy kontaktów?\n\nJeœli nie, mo¿esz nadal kontynuowaæ,\nale jako opisy kontaktów zostana u¿yte ich numery Gadu-Gadu.","Imp-GG",MB_YESNO|MB_ICONQUESTION)==IDNO)
  {
	  MessageBox(NULL,"Import archiwum zosta³ anulowany.","Imp-GG",MB_OK|MB_ICONINFORMATION);
	  IMLOG("SKOLIMA_ User abort");
	  return 0;
  }
  IMLOG("SKOLIMA_ Ruszam czytacza....");
  ArchReader* myReader = new GGReader();
  IMLOG("SKOLIMA_ Czytacz stworzony.");
  //wybór pliku do otwarcia
  char bufor[300];
	bufor[0]='\0';
	OPENFILENAME ofn;        
     memset (&ofn, 0, sizeof(OPENFILENAME));      
     ofn.lStructSize = sizeof(OPENFILENAME);
     ofn.hwndOwner = NULL;
	 ofn.lpstrFile = (LPSTR)bufor;
     ofn.nMaxFile = 299;
        ofn.lpstrFilter = "Archiwum Gadu-Gadu (archives.dat)\0archives.dat\0";
     ofn.nFilterIndex = 1;
     ofn.lpstrInitialDir = NULL;        
        ofn.Flags |=  OFN_EXPLORER;
        ofn.lpfnHook = NULL;
		ofn.lpstrTitle = "Wska¿ archiwum Gadu-Gadu do otwarcia:";
	//pokazuje okno, otwiera plik
		//i otwieramy archiwum
	if(!GetOpenFileName(&ofn)||!myReader->Prepare(&(ofn.lpstrFile)))//"archives.dat")) 
	{	
		IMLOG("SKOLIMA_ GG Reader nie moze otworzyc pliku");
		IMessage(IMI_ERROR , 0 , 0 , (int) "Nie mogê otworzyæ pliku archiwum Gadu-Gadu!\nImport anulowany.");
		return 0;
	}
  //sprawdŸmy UID archiwum
	char KonnektUID[65000],buff[65000];
	sprintf(KonnektUID,"%d",GETINT(1053));
	myReader->getOwnerUID(buff);
	if(strcmp(KonnektUID,buff)!=0)
	{
	  char buff1[65000];
	  sprintf(buff1,"Twój numer GG w Konnekcie (%s) ró¿ni siê od tego w archiwum (%s). Kontynuowaæ?",KonnektUID,buff);
	  if(MessageBox(NULL,buff1,"Imp-GG",MB_YESNO|MB_ICONQUESTION)==IDNO)
	  {
		MessageBox(NULL,"Import archiwum zosta³ anulowany.","Imp-GG",MB_OK|MB_ICONINFORMATION);
		IMLOG("SKOLIMA_ User abort");
		return 0;
	  }
	}
  //----------------------
  IMLOG("SKOLIMA_ Ruszam pisarza....");
  ArchWriter* myWriter = new KonnektWriter();
  IMLOG("SKOLIMA_ Pisarz stworzony.");
  int netID = NET_GG;
  if(!myWriter->Prepare(&netID)) 
	{	
		IMLOG("SKOLIMA_ Konnekt Writer nie chce wspolpracowac");
		IMessage(IMI_ERROR , 0 , 0 , (int) "Nie mogê dodaæ wiadomoœci do historii.\nImport anulowany.");
		return 0;
	}

  IMLOG("SKOLIMA_ Ruszam importera....");
  IMessage(IMI_INFORM , 0 , 0 , (int) "Rozpoczynam import. Nie wy³¹czaj Konnekta zanim wtyczka Imp-GG zakoñczy dzia³anie!");
  myResult addedMsgCount = ArchImporter::Run(myReader,myWriter);

  delete myReader;
  delete myWriter;

  IMLOG("SKOLIMA_ Zakoñczono. Teraz SMSy");
  //--------------------------------------------SMSY start  
  myResult addedSmsCount;
  addedSmsCount.added=addedSmsCount.ignored=0;
  myReader = new GGSmsReader();
  if(!myReader->Prepare(&(ofn.lpstrFile)))//"archives.dat")) 
	{	
		IMLOG("SKOLIMA_ GG SMSReader nie moze otworzyc pliku");
		IMessage(IMI_ERROR , 0 , 0 , (int) "Nie mogê otworzyæ pliku archiwum Gadu-Gadu!\nImport SMSów anulowany.");
	}
  else
  {
	IMLOG("SKOLIMA_ Ruszam pisarza SMS....");
	myWriter = new KonnektWriter();
	IMLOG("SKOLIMA_ Pisarz SMS stworzony.");

	netID = NET_SMS; //zmieniæ
	if(!myWriter->Prepare(&netID)) 
		{	
			IMLOG("SKOLIMA_ Konnekt Writer SMS nie chce wspolpracowac");
			IMessage(IMI_ERROR , 0 , 0 , (int) "Nie mogê dodaæ wiadomoœci SMS do historii.\nImport zakoñczony.");
		}
	else
	{
		IMLOG("SKOLIMA_ Ruszam importera....");
		addedSmsCount = ArchImporter::Run(myReader,myWriter);
		IMLOG("SKOLIMA_ Import SMS zakoñczony....");
	}
	delete myWriter;
  }

  delete myReader;
  //--------------------------------------------SMSY end
  IMLOG("SKOLIMA_ Zakoñczono. Import skoñczony");
  char result[65000];
  sprintf(result,"Odczyt archives.dat zakoñczony.\n\nNie uruchamiaj Imp-GG ponownie dla tego samego archiwum, bo wpisy w historii siê zduplikuj¹!\n\nDodano %d wiadomoœci, zignorowano %d.\nDodano %d SMSów, zignorowano %d.",addedMsgCount.added,addedMsgCount.ignored,addedSmsCount.added,addedSmsCount.ignored);

  Ctrl->IMessage(&sIMessage_plugOut(Ctrl->ID() , result , sIMessage_plugOut::enRestart::erYes , sIMessage_plugOut::euNowAndOnNextStart));

  return 1;
}

int IStart() {
  /* Tutaj wtyczkê uruchamiamy 
     ¯eby odebraæ ten komunikat (IM_START) trzeba podaæ jako typ wtyczki
     IMT_UI.
  */
  // Pobieramy ID wtyczki interfejsu (zawsze na pozycji 0)
  int UIHandle = ICMessage(IMC_PLUG_HANDLE , 0);
  // Logujemy nazwê wtyczki UI. Pobieramy j¹ wysy³aj¹c komunikat
  // bezpoœrednio do wtyczki.
  IMLOG("UI = \"%s\"" , IMessageDirect(IM_PLUG_NAME , UIHandle));

  //odczepiamy siê, jako osobny w¹tek
  DWORD myThread;
  HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Run, NULL, CREATE_SUSPENDED, &myThread);
  //z obnizonym priorytetem
  SetThreadPriority(threadHandle,THREAD_PRIORITY_BELOW_NORMAL);
  ResumeThread(threadHandle);
  CloseHandle(threadHandle);

  return 1;
}
int IEnd() {
  /* Tutaj wtyczkê wy³¹czamy */
  return 1;
}


int __stdcall IMessageProc(sIMessage_base * msgBase) {
 sIMessage_2params * msg = (msgBase->s_size>=sizeof(sIMessage_2params))?static_cast<sIMessage_2params*>(msgBase):0;
 switch (msgBase->id) {
    /* Wiadomoœci na które TRZEBA odpowiedzieæ */
    case IM_PLUG_NET:        return NET_IMPGG; // Zwracamy wartoœæ NET, która MUSI byæ ró¿na od 0 i UNIKATOWA!
    case IM_PLUG_TYPE:       return IMT_UI; // Zwracamy jakiego typu jest nasza wtyczka (które wiadomoœci bêdziemy obs³ugiwaæ)
    case IM_PLUG_VERSION:    return (int)myPLUGVERSION; // Wersja wtyczki tekstowo ...
    case IM_PLUG_SDKVERSION: return KONNEKT_SDK_V;  // Ta linijka jest wymagana!
    case IM_PLUG_SIG:        return (int)"SKOL1"; // Sygnaturka wtyczki (krótka, kilkuliterowa nazwa)
    case IM_PLUG_NAME:       return (int)myPLUGINNAME; // Pe³na nazwa wtyczki
    case IM_PLUG_NETNAME:    return 0; // Nazwa obs³ugiwanej sieci (o ile jak¹œ sieæ obs³uguje)
    case IM_PLUG_INIT:       Plug_Init(msg->p1,msg->p2);return 1;
    case IM_PLUG_DEINIT:     Plug_Deinit(msg->p1,msg->p2);return 1;

    case IM_START:           return IStart();


    /* Tutaj obs³ugujemy wszystkie pozosta³e wiadomoœci */
 }
	if (Ctrl) Ctrl->setError(IMERROR_NORESULT);
	return 0;
}
