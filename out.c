#include "vco.h"

void mmain(){
	{%a0:base[%a0:len] = '\0';
printf("%s%c", %a0:fst, SN %a1:fst ? '\n' : '\0');}
	copynry(%a0p, %a1p);
	copynry(%a0p, %a1p);
	printf("%d%c", %# %a0:fst, SN %a1:fst == 0 ? '\n' : '\0';
	%# %a0:fst += 1;
	{if(%# %a0:fst == %# %a1:fst) SN regs[flag].base = 0;
else if(%# %a0:fst < %# %a1:fst) SN regs[flag].base = 2;
else SN regs[flag].base = 2;}
	if(SN regs[flag].base == 2)
if(SN regs[flag].base == 2)
	aprintnry(%a0p, form, SN %a0:fst == 0);
	copynry(%a0p, %a1p);
	printf("%d%c", %# %a0:fst, SN %a1:fst == 0 ? '\n' : '\0';
	%# %a0:fst += 1;
	{if(%# %a0:fst == %# %a1:fst) SN regs[flag].base = 0;
else if(%# %a0:fst < %# %a1:fst) SN regs[flag].base = 2;
else SN regs[flag].base = 2;}
	if(SN regs[flag].base == 2)
goto %a;
	if(SN regs[flag].base == 0)
goto %a;
	{%a0:base[%a0:len] = '\0';
printf("%s%c", %a0:fst, SN %a1:fst ? '\n' : '\0');}
	aprintnry(%a0p, form, SN %a0:fst == 0);
}

int main(){
	initmac();
	mmain();
	freemac();
	return 0;
}
