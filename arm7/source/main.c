#include <nds.h>

volatile bool exitflag = false;

void powerButtonCB() { exitflag = true; }

void VcountHandler() { inputGetAndSend(); }

int main(void) {
	readUserSettings();
	ledBlink(0);
	
	irqInit();
	
	initClockIRQ();
	fifoInit();
	touchInit();
	
	SetYtrigger(80);
	
	installSystemFIFO();
	
	irqSet(IRQ_VCOUNT, VcountHandler);
	
	irqEnable(IRQ_VCOUNT);
	
	setPowerButtonCB(powerButtonCB);
	
	if (REG_SNDEXTCNT != 0) {
		i2cWriteRegister(0x4A, 0x12, 0x00);	// Press power-button for auto-reset
		i2cWriteRegister(0x4A, 0x70, 0x01);	// Bootflag = Warmboot/SkipHealthSafety
	}
	
	while(1) {
		swiWaitForVBlank();
		if (exitflag)break;
	}
	return 0;
}

