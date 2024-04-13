static unsigned char *get_mcs(int bitrate)
{
	struct yam_mcs *p;

	p = yam_data;
	while (p) {
		if (p->bitrate == bitrate)
			return p->bits;
		p = p->next;
	}

	/* Load predefined mcs data */
	switch (bitrate) {
	case 1200:
		/* setting predef as YAM_1200 for loading predef 1200 mcs */
		return add_mcs(NULL, bitrate, YAM_1200);
	default:
		/* setting predef as YAM_9600 for loading predef 9600 mcs */
		return add_mcs(NULL, bitrate, YAM_9600);
	}
}