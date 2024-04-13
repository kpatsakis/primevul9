ReadChannelMasks(unsigned int       *tmp,
	Bitmap_Channel *masks,
	unsigned int          channels)
{
	unsigned int i;

	for (i = 0; i < channels; i++)
	{
		unsigned int mask;
		int    nbits, offset, bit;

		mask = tmp[i];
		masks[i].mask = mask;
		nbits = 0;
		offset = -1;

		for (bit = 0; bit < 32; bit++)
		{
			if (mask & 1)
			{
				nbits++;
				if (offset == -1)
					offset = bit;
			}

			mask = mask >> 1;
		}

		masks[i].shiftin = offset;
		masks[i].max_value = (float)((1 << nbits) - 1);

#ifdef _DEBUG
		LOG4("Channel %d mask %08x in %d max_val %d\n",
			i, masks[i].mask, masks[i].shiftin, (int)masks[i].max_value);
#endif
	}

	return TRUE;
}