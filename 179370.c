premultiply_data (png_structp   png,
                  png_row_infop row_info,
                  png_bytep     data)
{
	unsigned int i;

	for (i = 0; i < row_info->rowbytes; i += 4) {
		uint8_t *base  = &data[i];
		uint8_t  alpha = base[3];
		uint32_t p;

		if (alpha == 0) {
			p = 0;
		} else {
			uint8_t red   = base[0];
			uint8_t green = base[1];
			uint8_t blue  = base[2];

			if (alpha != 0xff) {
				red   = multiply_alpha (alpha, red);
				green = multiply_alpha (alpha, green);
				blue  = multiply_alpha (alpha, blue);
			}
			p = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
		}
		memcpy (base, &p, sizeof (uint32_t));
	}
}