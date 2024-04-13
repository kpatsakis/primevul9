mcs_recv(uint16 * channel, uint8 * rdpver)
{
	uint8 opcode, appid, length;
	STREAM s;

	s = iso_recv(rdpver);
	if (s == NULL)
		return NULL;
	if (rdpver != NULL)
		if (*rdpver != 3)
			return s;
	in_uint8(s, opcode);
	appid = opcode >> 2;
	if (appid != MCS_SDIN)
	{
		if (appid != MCS_DPUM)
		{
			error("expected data, got %d\n", opcode);
		}
		return NULL;
	}
	in_uint8s(s, 2);	/* userid */
	in_uint16_be(s, *channel);
	in_uint8s(s, 1);	/* flags */
	in_uint8(s, length);
	if (length & 0x80)
		in_uint8s(s, 1);	/* second byte of length */
	return s;
}