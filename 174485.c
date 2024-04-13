rdp_process_server_caps(STREAM s, uint16 length)
{
	int n;
	uint8 *next, *start;
	uint16 ncapsets, capset_type, capset_length;

	start = s->p;

	in_uint16_le(s, ncapsets);
	in_uint8s(s, 2);	/* pad */

	for (n = 0; n < ncapsets; n++)
	{
		if (s->p > start + length)
			return;

		in_uint16_le(s, capset_type);
		in_uint16_le(s, capset_length);

		next = s->p + capset_length - 4;

		switch (capset_type)
		{
			case RDP_CAPSET_GENERAL:
				rdp_process_general_caps(s);
				break;

			case RDP_CAPSET_BITMAP:
				rdp_process_bitmap_caps(s);
				break;
		}

		s->p = next;
	}
}