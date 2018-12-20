#include "VGMHardwarePlayer.h"

#define I2C_CLK_W(s) 
#define I2C_DAT_W(s)
#define I2C_DAT_R(s)
#define NOP(n)

#define I2C_INIT() I2C_CLK(1) I2C_DAT(1)

#define tBUF 60
#define tHD_STA 10
#define tHD_DAT 20
#define tSU_DAT 30
#define tHigh 40
#define tVD_DAT 50
#define tSU_STOP 60

#define I2C_START() NOP(tBUF/2) I2C_DAT_W(0) NOP(tHD_STA) I2C_CLK_W(0) NOP(tHD_DAT)
#define I2C_BIT(bit) I2C_DAT_W(bit) NOP(tSU_DAT) I2C_CLK_W(1) NOP(tHigh) I2C_CLK_W(0) NOP(tVD_DAT)
#define I2C_WAITACK(rval) I2C_DAT_W(1) NOP(tSU_DAT) I2C_DAT_R(rval) I2C_CLK_W(1) NOP(tHigh) I2C_CLK_W(0) NOP(tVD_DAT)
#define I2C_END() I2C_CLK_W(1) NOP(tSU_STOP) I2C_DAT_W(1) NOP(tBUF/2)

#define I2C_WRITE_BYTE(rval, data) for(i=0; i<8; i++){ I2C_BIT( (data >>= 1) & 0x01 } I2C_WAITACK(rval)


VGMHardwarePlayer::VGMHardwarePlayer()
: VGMDataObverser()
{
}

VGMHardwarePlayer::~VGMHardwarePlayer()
{
}

void VGMHardwarePlayer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMHardwarePlayer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();
}

void VGMHardwarePlayer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();
}

void VGMHardwarePlayer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMHardwarePlayer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMHardwarePlayer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMHardwarePlayer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMHardwarePlayer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}