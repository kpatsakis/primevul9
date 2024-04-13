void png_convert_4(
	unsigned pix_width, unsigned pix_height, unsigned pix_pixel, unsigned char* pix_ptr, unsigned pix_scanline,
	unsigned char* pal_ptr, unsigned pal_size,
	unsigned char** dst_ptr, unsigned* dst_pixel, unsigned* dst_scanline)
{
	*dst_pixel = 4;
	*dst_scanline = 4 * pix_width;
	*dst_ptr = (unsigned char*)malloc(*dst_scanline * pix_height);

	if (pix_pixel == 3) {
		unsigned i, j;
		for(i=0;i<pix_height;++i) {
			const unsigned char* p0 = pix_ptr + i * pix_scanline;
			unsigned char* p1 = *dst_ptr + i * *dst_scanline;
			for(j=0;j<pix_width;++j) {
				p1[0] = p0[0];
				p1[1] = p0[1];
				p1[2] = p0[2];
				p1[3] = 0xFF;
				p0 += 3;
				p1 += 4;
			}
		}
	} else if (pix_pixel == 1) {
		unsigned i, j;
		for(i=0;i<pix_height;++i) {
			const unsigned char* p0 = pix_ptr + i * pix_scanline;
			unsigned char* p1 = *dst_ptr + i * *dst_scanline;
			for(j=0;j<pix_width;++j) {
				unsigned char* c = &pal_ptr[p0[0]*3];
				p1[0] = c[0];
				p1[1] = c[1];
				p1[2] = c[2];
				p1[3] = 0xFF;
				p0 += 1;
				p1 += 4;
			}
		}
	} else if (pix_pixel == 4) {
		unsigned i;
		for(i=0;i<pix_height;++i) {
			const unsigned char* p0 = pix_ptr + i * pix_scanline;
			unsigned char* p1 = *dst_ptr + i * *dst_scanline;
			memcpy(p1, p0, *dst_scanline);
		}
	} else {
		throw error_unsupported() << "Unsupported format";
	}
}