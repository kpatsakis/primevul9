convert_bytes_to_data (png_structp png, png_row_infop row_info, png_bytep data)
{
	unsigned int i;

	for (i = 0; i < row_info->rowbytes; i += 4) {
		uint8_t *base  = &data[i];
		uint8_t  red   = base[0];
		uint8_t  green = base[1];
		uint8_t  blue  = base[2];
		uint32_t pixel;

		pixel = (0xff << 24) | (red << 16) | (green << 8) | (blue << 0);
		memcpy (base, &pixel, sizeof (uint32_t));
	}
}