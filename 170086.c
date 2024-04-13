lsb_32 (guchar *src)
{
	return src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24);
}