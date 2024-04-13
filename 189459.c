void png_write(adv_fz* f, unsigned pix_width, unsigned pix_height, unsigned pix_pixel, unsigned char* pix_ptr, unsigned pix_scanline, unsigned char* pal_ptr, unsigned pal_size, unsigned char* rns_ptr, unsigned rns_size, shrink_t level)
{
	unsigned char ihdr[13];
	data_ptr z_ptr;
	unsigned z_size;

	if (adv_png_write_signature(f, 0) != 0) {
		throw_png_error();
	}

	be_uint32_write(ihdr + 0, pix_width);
	be_uint32_write(ihdr + 4, pix_height);
	ihdr[8] = 8; /* bit depth */
	if (pix_pixel == 1)
		ihdr[9] = 3; /* color type */
	else if (pix_pixel == 3)
		ihdr[9] = 2; /* color type */
	else if (pix_pixel == 4)
		ihdr[9] = 6; /* color type */
	else
		throw error() << "Invalid format";

	ihdr[10] = 0; /* compression */
	ihdr[11] = 0; /* filter */
	ihdr[12] = 0; /* interlace */

	if (adv_png_write_chunk(f, ADV_PNG_CN_IHDR, ihdr, sizeof(ihdr), 0) != 0) {
		throw_png_error();
	}

	if (pal_size) {
		if (adv_png_write_chunk(f, ADV_PNG_CN_PLTE, pal_ptr, pal_size, 0) != 0) {
			throw_png_error();
		}
	}

	if (rns_size) {
		if (adv_png_write_chunk(f, ADV_PNG_CN_tRNS, rns_ptr, rns_size, 0) != 0) {
			throw_png_error();
		}
	}

	png_compress(level, z_ptr, z_size, pix_ptr, pix_scanline, pix_pixel, 0, 0, pix_width, pix_height);

	if (adv_png_write_chunk(f, ADV_PNG_CN_IDAT, z_ptr, z_size, 0) != 0) {
		throw_png_error();
	}

	if (adv_png_write_chunk(f, ADV_PNG_CN_IEND, 0, 0, 0) != 0) {
		throw_png_error();
	}
}