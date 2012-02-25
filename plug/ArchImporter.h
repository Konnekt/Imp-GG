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
#include "archwriter.h"

struct myResult
{
	int added;
	int ignored;
};

class ArchImporter
{
public:
	ArchImporter(void);
	~ArchImporter(void);
	// przed wywo³aniem Reader i Writer musz¹ ju¿ miec wywo³ane Prepare() !
	//zwraca strukturê myResult-iloœæ dodanych i zignorowanych wiadomoœci
	static myResult Run(ArchReader* Reader, ArchWriter* Writer);
};

