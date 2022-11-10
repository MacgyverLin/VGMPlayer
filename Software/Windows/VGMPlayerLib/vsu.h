#ifndef __VB_VSU_H
#define __VB_VSU_H

void VSU_Write(UINT8 ChipID, UINT32 A, UINT8 V);

void vsu_stream_update(UINT8 ChipID, stream_sample_t **outputs, int samples, stream_sample_t** channeoutputs, int channelcount);

int device_start_vsu(UINT8 ChipID, int clock, UINT8 CHIP_SAMPLING_MODE, INT32 CHIP_SAMPLE_RATE, UINT32 SampleRate);
void device_stop_vsu(UINT8 ChipID);
void device_reset_vsu(UINT8 ChipID);

//void vsu_set_options(UINT16 Options);
void vsu_set_mute_mask(UINT8 ChipID, UINT32 MuteMask);

#endif	// __VB_VSU_H
