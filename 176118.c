adv_error adv_png_read_rns(
	unsigned* pix_width, unsigned* pix_height, unsigned* pix_pixel,
	unsigned char** dat_ptr, unsigned* dat_size,
	unsigned char** pix_ptr, unsigned* pix_scanline,
	unsigned char** pal_ptr, unsigned* pal_size,
	unsigned char** rns_ptr, unsigned* rns_size,
	adv_fz* f)
{
	unsigned char* data;
	unsigned type;
	unsigned size;

	if (adv_png_read_signature(f) != 0) {
		goto err;
	}

	do {
		if (adv_png_read_chunk(f, &data, &size, &type) != 0) {
			goto err;
		}

		switch (type) {
			case ADV_PNG_CN_IHDR :
				if (adv_png_read_ihdr(pix_width, pix_height, pix_pixel, dat_ptr, dat_size, pix_ptr, pix_scanline, pal_ptr, pal_size, rns_ptr, rns_size, f, data, size) != 0)
					goto err_data;
				free(data);
				return 0;
			default :
				/* ancillary bit. bit 5 of first byte. 0 (uppercase) = critical, 1 (lowercase) = ancillary. */
				if ((type & 0x20000000) == 0) {
					char buf[4];
					be_uint32_write(buf, type);
					error_unsupported_set("Unsupported critical chunk '%c%c%c%c'", buf[0], buf[1], buf[2], buf[3]);
					goto err_data;
				}
				/* ignored */
				break;
		}

		free(data);

	} while (type != ADV_PNG_CN_IEND);

	error_set("Invalid PNG file");
	return -1;

err_data:
	free(data);
err:
	return -1;
}