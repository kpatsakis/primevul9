static u8 add_1mod(u8 byte, u32 reg)
{
	if (is_ereg(reg))
		byte |= 1;
	return byte;
}