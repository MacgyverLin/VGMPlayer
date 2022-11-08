#include "mamedef.h"

static UINT8 YM2413_EMU_CORE;
// static UINT32 OptArr[0x10];

void ym2413opl_set_emu_core(UINT8 Emulator)
{
	YM2413_EMU_CORE = (Emulator < 0x02) ? Emulator : 0x00;

	return;
}