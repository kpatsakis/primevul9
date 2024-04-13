adv_error adv_png_read(
	unsigned* pix_width, unsigned* pix_height, unsigned* pix_pixel,
	unsigned char** dat_ptr, unsigned* dat_size,
	unsigned char** pix_ptr, unsigned* pix_scanline,
	unsigned char** pal_ptr, unsigned* pal_size,
	adv_fz* f)
{
	adv_error r;
	unsigned char* rns_ptr;
	unsigned rns_size;

	r = adv_png_read_rns(pix_width, pix_height, pix_pixel, dat_ptr, dat_size, pix_ptr, pix_scanline, pal_ptr, pal_size, &rns_ptr, &rns_size, f);

	if (r == 0)
		free(rns_ptr);

	return r;
}