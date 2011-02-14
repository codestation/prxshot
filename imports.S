	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemForKernel",0x00010011,0x00010005
	STUB_FUNC 0x0D547E7F,sceKernelGetUMDData
	STUB_END

	STUB_START "sceSuspendForKernel",0x40000000,0x00020005
    STUB_FUNC  0xA14F40B2,sceKernelVolatileMemTryLock
    STUB_FUNC  0xA569E425,sceKernelVolatileMemUnlock
    STUB_END
