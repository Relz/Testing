#include "stdafx.h"
#include "UTF16.h"

void UseUTF8()
{
	_setmode(_fileno(stdout), _O_U8TEXT);
	_setmode(_fileno(stdin), _O_U8TEXT);
}
