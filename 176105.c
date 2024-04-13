adv_error adv_png_write_raw(
	unsigned pix_width, unsigned pix_height, unsigned pix_pixel,
	const unsigned char* pix_ptr, int pix_pixel_pitch, int pix_scanline_pitch,
	const unsigned char* pal_ptr, unsigned pal_size,
	const unsigned char* rns_ptr, unsigned rns_size,
	adv_bool fast,
	adv_fz* f, unsigned* count)
{
	unsigned color;
	unsigned depth;

	if (pix_pixel == 1 && pal_size != 0) {
		color = 3;
		depth = 8;
	} else if (pix_pixel == 1 && pal_size == 0) {
		color = 1;
		depth = 8;
	} else if (pix_pixel == 3 && pal_size == 0) {
		color = 2;
		depth = 8;
	} else if (pix_pixel == 4 && pal_size == 0) {
		color = 6;
		depth = 8;
	} else {
		error_unsupported_set("Unsupported bit depth/color");
		goto err;
	}

	if (adv_png_write_ihdr(pix_width, pix_height, depth, color, f, count) != 0) {
		goto err;
	}

	if (pal_size) {
		if (adv_png_write_chunk(f, ADV_PNG_CN_PLTE, pal_ptr, pal_size, count) != 0) {
			goto err;
		}
	}

	if (rns_size) {
		if (adv_png_write_chunk(f, ADV_PNG_CN_tRNS, rns_ptr, rns_size, count) != 0) {
			goto err;
		}
	}

	if (adv_png_write_idat(pix_width, pix_height, pix_pixel, pix_ptr, pix_pixel_pitch, pix_scanline_pitch, 0, f, count) != 0) {
		goto err;
	}

	if (adv_png_write_iend(f, count) != 0) {
		goto err;
	}

	return 0;

err:
	return -1;
}