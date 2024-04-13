adv_error adv_png_write_ihdr(
	unsigned pix_width, unsigned pix_height,
	unsigned pix_depth, unsigned pix_type,
	adv_fz* f, unsigned* count)
{
	uint8 ihdr[13];

	be_uint32_write(ihdr, pix_width);
	be_uint32_write(ihdr+4, pix_height);

	ihdr[8] = pix_depth;
	ihdr[9] = pix_type;
	ihdr[10] = 0; /* compression */
	ihdr[11] = 0; /* filter */
	ihdr[12] = 0; /* interlace */

	if (adv_png_write_chunk(f, ADV_PNG_CN_IHDR, ihdr, 13, count)!=0)
		return -1;

	return 0;
}