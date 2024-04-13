static int dns_name_length(unsigned char *buf)
{
	if ((buf[0] & NS_CMPRSFLGS) == NS_CMPRSFLGS) /* compressed name */
		return 2;
	return strlen((char *)buf) + 1;
}