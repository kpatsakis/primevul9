ptp_pack_EOS_CustomFuncEx (PTPParams* params, unsigned char* data, char* str)
{
	uint32_t s = strtoul(str, NULL, 16);
	uint32_t n = s/4, i, v;

	if (!data)
		return s;

	for (i=0; i<n; i++)
	{
		v = strtoul(str, &str, 16);
		str++; /* skip the ',' delimiter */
		htod32a(data + i*4, v);
	}

	return s;
}