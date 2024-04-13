process_compressed_8x8_brush_data(uint8 * in, uint8 * out, int Bpp)
{
	int x, y, pal_index, in_index, shift, do2, i;
	uint8 *pal;

	in_index = 0;
	pal = in + 16;
	/* read it bottom up */
	for (y = 7; y >= 0; y--)
	{
		/* 2 bytes per row */
		x = 0;
		for (do2 = 0; do2 < 2; do2++)
		{
			/* 4 pixels per byte */
			shift = 6;
			while (shift >= 0)
			{
				pal_index = (in[in_index] >> shift) & 3;
				/* size of palette entries depends on Bpp */
				for (i = 0; i < Bpp; i++)
				{
					out[(y * 8 + x) * Bpp + i] = pal[pal_index * Bpp + i];
				}
				x++;
				shift -= 2;
			}
			in_index++;
		}
	}
}