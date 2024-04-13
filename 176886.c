static u8 add_2mod(u8 byte, u32 r1, u32 r2)
{
	if (is_ereg(r1))
		byte |= 1;
	if (is_ereg(r2))
		byte |= 4;
	return byte;
}