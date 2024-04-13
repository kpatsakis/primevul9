char *formatName(int subsamp, int cs, char *buf)
{
	if(cs==TJCS_YCbCr) return (char *)subNameLong[subsamp];
	else if(cs==TJCS_YCCK)
	{
		snprintf(buf, 80, "%s %s", csName[cs], subNameLong[subsamp]);
		return buf;
	}
	else return (char *)csName[cs];
}