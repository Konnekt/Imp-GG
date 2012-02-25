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

#include ".\archimporter.h"

ArchImporter::ArchImporter(void)
{
}

ArchImporter::~ArchImporter(void)
{
}

// przed wywo³aniem Reader i Writer musz¹ ju¿ miec wywo³ane Prepare() !
//zwraca strukturê myResult-iloœæ dodanych i zignorowanych wiadomoœci
myResult ArchImporter::Run(ArchReader* Reader, ArchWriter* Writer)
{
	myResult result;
	result.added = result.ignored = 0;
	char bufor[65000],bufor1[65000],bufor2[65000],bufor3[65000];
	Writer->setOwnerUID(Reader->getOwnerUID(bufor1));
	while(Reader->getNext())
	{
		while(Reader->msgHasReceivers())Writer->setMsgReceiverUID(Reader->getMsgReceiverUID(bufor2));
		Writer->setMsgSenderUID(Reader->getMsgSenderUID(bufor3));
		Writer->setMsgText(Reader->getMsgText(bufor));
		Writer->setMsgTime(Reader->getMsgTime());
		Writer->setMsgWasSent(Reader->getMsgWasSent());
		if(Writer->addMsg())result.added++; else result.ignored++;
	}
	result.ignored += Reader->getNumberOfIgnored();
	return result;
}
