#pragma once

void c6280_w(UINT8 ChipID, offs_t offset, UINT8 data);
UINT8 c6280_r(UINT8 ChipID, offs_t offset);

void c6280_update(UINT8 ChipID, stream_sample_t **outputs, int samples, stream_sample_t** channeoutputs, int channelcount);
int device_start_c6280(UINT8 ChipID, int clock, UINT8 CHIP_SAMPLING_MODE, INT32 CHIP_SAMPLE_RATE, UINT32 SampleRate);
void device_stop_c6280(UINT8 ChipID);
void device_reset_c6280(UINT8 ChipID);

void c6280_set_emu_core(UINT8 Emulator);
void c6280_set_mute_mask(UINT8 ChipID, UINT32 MuteMask);
