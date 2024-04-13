void adv_png_expand_1(unsigned width, unsigned height, unsigned char* ptr)
{
	unsigned i, j;
	unsigned char* p8 = ptr + height * (width + 1) - 1;
	unsigned char* p1 = ptr + height * (width / 8 + 1) - 1;

	width /= 8;
	for(i=0;i<height;++i) {
		for(j=0;j<width;++j) {
			unsigned char v = *p1;
			*p8-- = v & 0x1;
			*p8-- = (v >> 1) & 0x1;
			*p8-- = (v >> 2) & 0x1;
			*p8-- = (v >> 3) & 0x1;
			*p8-- = (v >> 4) & 0x1;
			*p8-- = (v >> 5) & 0x1;
			*p8-- = (v >> 6) & 0x1;
			*p8-- = v >> 7;
			--p1;
		}
		--p8;
		--p1;
	}
}