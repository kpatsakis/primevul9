void png_compress_delta(shrink_t level, data_ptr& out_ptr, unsigned& out_size, const unsigned char* img_ptr, unsigned img_scanline, unsigned img_pixel, const unsigned char* prev_ptr, unsigned prev_scanline, unsigned x, unsigned y, unsigned dx, unsigned dy)
{
	data_ptr fil_ptr;
	unsigned fil_size;
	unsigned fil_scanline;
	data_ptr z_ptr;
	unsigned z_size;
	unsigned i;
	unsigned char* p0;

	fil_scanline = dx * img_pixel + 1;
	fil_size = dy * fil_scanline;
	z_size = oversize_zlib(fil_size);

	fil_ptr = data_alloc(fil_size);
	z_ptr = data_alloc(z_size);

	p0 = fil_ptr;

	for(i=0;i<dy;++i) {
		unsigned j;
		const unsigned char* p1 = &img_ptr[x * img_pixel + (i+y) * img_scanline];
		const unsigned char* p2 = &prev_ptr[x * img_pixel + (i+y) * prev_scanline];

		*p0++ = 0;
		for(j=0;j<dx*img_pixel;++j)
			*p0++ = *p1++ - *p2++;
	}

	assert(p0 == fil_ptr + fil_size);

	if (!compress_zlib(level, z_ptr, z_size, fil_ptr, fil_size)) {
		throw error() << "Failed compression";
	}

	out_ptr = z_ptr;
	out_size = z_size;
}