static u8 add_1reg(u8 byte, u32 dst_reg)
{
	return byte + reg2hex[dst_reg];
}