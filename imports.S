	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemForKernel",0x00010011,0x00010005
	STUB_FUNC 0x0D547E7F,sceKernelGetUMDData
	STUB_END
