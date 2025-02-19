static int ras_putdatastd(jas_stream_t *out, ras_hdr_t *hdr, jas_image_t *image,  int numcmpts, int *cmpts)
{
	int rowsize;
	int pad;
	unsigned int z;
	int nz;
	int c;
	int x;
	int y;
	int v;
	jas_matrix_t *data[3];
	int i;

	assert(numcmpts <= 3);

	if (RAS_ISRGB(hdr) && numcmpts < 3) {
		/* need 3 components for RGB */
		return -1;
	}

	for (i = 0; i < 3; ++i) {
		data[i] = 0;
	}

	for (i = 0; i < numcmpts; ++i) {
		if (!(data[i] = jas_matrix_create(jas_image_height(image),
		  jas_image_width(image)))) {
			goto error;
		}
	}

	rowsize = RAS_ROWSIZE(hdr);
	pad = rowsize - (hdr->width * hdr->depth + 7) / 8;

	hdr->length = hdr->height * rowsize;

	for (y = 0; y < hdr->height; y++) {
		for (i = 0; i < numcmpts; ++i) {
			if (jas_image_readcmpt(image, cmpts[i], 0, y,
					jas_image_width(image), 1, data[i])) {
				goto error;
			}
		}
		z = 0;
		nz = 0;
		for (x = 0; x < hdr->width; x++) {
			z <<= hdr->depth;
			if (RAS_ISRGB(hdr)) {
				v = RAS_RED((jas_matrix_getv(data[0], x))) |
				  RAS_GREEN((jas_matrix_getv(data[1], x))) |
				  RAS_BLUE((jas_matrix_getv(data[2], x)));
			} else {
				v = (jas_matrix_getv(data[0], x));
			}
			z |= v & RAS_ONES(hdr->depth);
			nz += hdr->depth;
			while (nz >= 8) {
				c = (z >> (nz - 8)) & 0xff;
				if (jas_stream_putc(out, c) == EOF) {
					goto error;
				}
				nz -= 8;
				z &= RAS_ONES(nz);
			}
		}
		if (nz > 0) {
			c = (z >> (8 - nz)) & RAS_ONES(nz);
			if (jas_stream_putc(out, c) == EOF) {
				goto error;
			}
		}
		if (pad % 2) {
			if (jas_stream_putc(out, 0) == EOF) {
				goto error;
			}
		}
	}

	for (i = 0; i < numcmpts; ++i) {
		jas_matrix_destroy(data[i]);
		data[i] = 0;
	}

	return 0;

error:
	for (i = 0; i < numcmpts; ++i) {
		if (data[i]) {
			jas_matrix_destroy(data[i]);
		}
	}
	return -1;
}