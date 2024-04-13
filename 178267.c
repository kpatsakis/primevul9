static unsigned int get_msg_len(unsigned char *buf)
{
	return buf[0]<<8 | buf[1];
}