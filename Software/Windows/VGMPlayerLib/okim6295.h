#pragma once

/* an interface for the OKIM6295 and similar chips */

/*
  Note about the playback frequency: the external clock is internally divided,
  depending on pin 7, by 132 (high) or 165 (low).
*/
/*typedef struct _okim6295_interface okim6295_interface;
struct _okim6295_interface
{
	int pin7;
};

extern const okim6295_interface okim6295_interface_pin7high;
extern const okim6295_interface okim6295_interface_pin7low;*/



//void okim6295_set_bank_base(running_device *device, int base);
//void okim6295_set_pin7(running_device *device, int pin7);

void okim6295_update(UINT8 ChipID, stream_sample_t **outputs, int samples, stream_sample_t** channeoutputs, int channelcount);
int device_start_okim6295(UINT8 ChipID, int clock, UINT8 CHIP_SAMPLING_MODE, INT32 CHIP_SAMPLE_RATE, UINT32 SampleRate);
void device_stop_okim6295(UINT8 ChipID);
void device_reset_okim6295(UINT8 ChipID);

//READ8_DEVICE_HANDLER( okim6295_r );
//WRITE8_DEVICE_HANDLER( okim6295_w );
void okim6295_w(UINT8 ChipID, offs_t offset, UINT8 data);

void okim6295_write_rom(UINT8 ChipID, offs_t ROMSize, offs_t DataStart, offs_t DataLength,
						const UINT8* ROMData);
void okim6295_set_mute_mask(UINT8 ChipID, UINT32 MuteMask);
void okim6295_set_srchg_cb(UINT8 ChipID, SRATE_CALLBACK CallbackFunc, void* DataPtr);


/*
    To help the various custom ADPCM generators out there,
    the following routines may be used.
*/
struct adpcm_state
{
	INT32	signal;
	INT32	step;
};
void reset_adpcm(struct adpcm_state *state);
INT16 clock_adpcm(struct adpcm_state *state, UINT8 nibble);

//DEVICE_GET_INFO( okim6295 );
//#define SOUND_OKIM6295 DEVICE_GET_INFO_NAME( okim6295 )
