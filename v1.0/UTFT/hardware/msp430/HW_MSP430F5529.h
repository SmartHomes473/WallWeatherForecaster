// *** Hardwarespecific functions ***
void UTFT::_set_direction_registers(byte mode)
{
	if (mode!=LATCHED_16)
	{
		P6DIR |= 0xFF;
		P7DIR |= 0xFF;
		P3DIR |= 0xFF;
		if (mode==16) {
			#pragma message("_set_direction_registers mode 16 pins not defined!")
		}
	}
	else
	{
		asm("nop");		// Mode is unsupported
	}
}

// Set 8-bit parallel interface pins: 
// 	DB1 - D7 = GND
//	DB8 = P6.0
//	DB9 = P6.1
//	DB10 = P6.2
//	DB11 = P6.3
//	DB12 = P6.4
//	DB13 = P7.0
//	DB14 = P3.6
//	DB15 = P3.5 
inline void write_byte_hi(unsigned char ch)
{
	unsigned char tmp = 0x00;
	P6OUT = ch& 0x1F;
	/*P6_0 = ch & 0x1;
	P6_1 = (ch >> 1) & 0x1;
	P6_2 = (ch >> 2) & 0x1;
	P6_3 = (ch >> 3) & 0x1;
	P6_4 = (ch >> 4) & 0x1;*/
	
	P7OUT = (ch >> 5) & 0x1;
	//P7_0 = (ch >> 5) & 0x1;
	
	tmp = ((ch >> 6) & 0x1) << 6;
	tmp = ((ch >> 7) & 0x1) << 5;
	P3OUT = tmp;
	
	/*P3_6 = (ch >> 6) & 0x1;
	P3_5 = (ch >> 7) & 0x1;*/
}
