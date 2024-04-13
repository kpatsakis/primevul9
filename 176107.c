adv_error adv_png_write_rns(
	unsigned pix_width, unsigned pix_height, unsigned pix_pixel,
	const unsigned char* pix_ptr, int pix_pixel_pitch, int pix_scanline_pitch,
	const unsigned char* pal_ptr, unsigned pal_size,
	const unsigned char* rns_ptr, unsigned rns_size,
	adv_bool fast,
	adv_fz* f, unsigned* count)
{
	if (adv_png_write_signature(f, count) != 0) {
		return -1;
	}

	return adv_png_write_raw(
		pix_width, pix_height, pix_pixel,
		pix_ptr, pix_pixel_pitch, pix_scanline_pitch,
		pal_ptr, pal_size,
		rns_ptr, rns_size,
		fast,
		f, count
	);
}