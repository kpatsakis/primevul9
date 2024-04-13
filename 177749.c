static inline void phar_write_16(char buffer[2], php_uint32 value)
{
	buffer[1] = (unsigned char) ((value & 0xff00) >> 8);
	buffer[0] = (unsigned char) (value & 0xff);
}