adv_error adv_png_write_idat(
	unsigned pix_width, unsigned pix_height, unsigned pix_pixel,
	const uint8* pix_ptr, int pix_pixel_pitch, int pix_scanline_pitch,
	adv_bool fast,
	adv_fz* f, unsigned* count)
{
	uint8* z_ptr;
	uint8* r_ptr;
	unsigned char filter;
	unsigned long z_size;
	unsigned res_size;
	const uint8* p;
	unsigned i;
	int method;
	z_stream z;
	int r;

	z_size = pix_height * (pix_width * (pix_pixel+1)) * 103 / 100 + 12;

	if (pix_pixel_pitch != pix_pixel) {
		r_ptr = (uint8*)malloc(pix_width * pix_pixel);
		if (!r_ptr)
			goto err;
	} else {
		r_ptr = 0;
	}

	z_ptr = (uint8*)malloc(z_size);
	if (!z_ptr)
		goto err_row;

	if (fast)
		method = Z_BEST_SPEED;
	else
		method = Z_DEFAULT_COMPRESSION;

	z.zalloc = 0;
	z.zfree = 0;
	z.next_out = z_ptr;
	z.avail_out = z_size;
	z.next_in = 0;
	z.avail_in = 0;

	p = pix_ptr;
	filter = 0;

	r = deflateInit(&z, method);

	for(i=0;i<pix_height;++i) {
		z.next_in = &filter; /* filter byte */
		z.avail_in = 1;

		r = deflate(&z, Z_NO_FLUSH);
		if (r != Z_OK) {
			error_set("Error compressing data");
			goto err_free;
		}

		if (r_ptr) {
			unsigned char* r = r_ptr;
			unsigned j;
			for(j=0;j<pix_width;++j) {
				unsigned k;
				for(k=0;k<pix_pixel;++k) {
					*r++ = *p++;
				}
				p += pix_pixel_pitch - pix_pixel;
			}
			z.next_in = r_ptr; /* pixel data */
			z.avail_in = pix_width * pix_pixel;
			p += pix_scanline_pitch - pix_width * pix_pixel_pitch;
		} else {
			z.next_in = (uint8*)p; /* pixel data */
			z.avail_in = pix_width * pix_pixel;
			p += pix_scanline_pitch;
		}

		r = deflate(&z, Z_NO_FLUSH);
		if (r != Z_OK) {
			error_set("Error compressing data");
			goto err_free;
		}
	}

	r = deflate(&z, Z_FINISH);
	if (r != Z_STREAM_END) {
		error_set("Error compressing data");
		goto err_free;
	}

	res_size = z.total_out;

	r = deflateEnd(&z);
	if (r != Z_OK) {
		error_set("Error compressing data");
		goto err_free;
	}

	if (adv_png_write_chunk(f, ADV_PNG_CN_IDAT, z_ptr, res_size, count)!=0)
		goto err_free;

	free(z_ptr);
	if (r_ptr)
		free(r_ptr);

	return 0;

err_free:
	free(z_ptr);
err_row:
	if (r_ptr)
		free(r_ptr);
err:
	return -1;
}