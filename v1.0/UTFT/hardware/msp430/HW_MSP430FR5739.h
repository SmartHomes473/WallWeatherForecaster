// *** Hardwarespecific functions ***
void UTFT::_set_direction_registers(byte mode)
{
	if (mode!=LATCHED_16)
	{
		P1DIR |= 0xFF;
		if (mode==16) {
			#pragma message("_set_direction_registers mode 16 pins not defined!")
		}
	}
	else
	{
		asm("nop");		// Mode is unsupported
	}
}

// Set 8-bit parallel interface pins: D8-D15 = P1.0-P1.7
inline void write_byte_hi(unsigned char ch)
{
	P1OUT = ch;
}
