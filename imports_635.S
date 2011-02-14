 	.set noreorder

#include "pspstub.s"

	STUB_START	"sceDisplay_driver",0x00010000,0x00020005
	STUB_FUNC	0x08A10838,sceDisplayGetFrameBuf
	STUB_FUNC	0xC30D327D,sceDisplayWaitVblankStart
	STUB_END

	STUB_START	"sceCtrl_driver",0x00010000,0x00010005
	STUB_FUNC	0x18654FC0,sceCtrlPeekBufferPositive
	STUB_END

	STUB_START	"SysMemForKernel",0x00010000,0x00030005
	STUB_FUNC	0x4621A9CC,sceKernelAllocPartitionMemory
	STUB_FUNC	0x8FDAFC4C,sceKernelFreePartitionMemory
	STUB_FUNC	0x52B54B93,sceKernelGetBlockHeadAddr
	STUB_END
