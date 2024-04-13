void adv_png_expand_2(unsigned width, unsigned height, unsigned char* ptr)
{
	unsigned i, j;
	unsigned char* p8 = ptr + height * (width + 1) - 1;
	unsigned char* p2 = ptr + height * (width / 4 + 1) - 1;

	width /= 4;
	for(i=0;i<height;++i) {
		for(j=0;j<width;++j) {
			unsigned char v = *p2;
			*p8-- = v & 0x3;
			*p8-- = (v >> 2) & 0x3;
			*p8-- = (v >> 4) & 0x3;
			*p8-- = v >> 6;
			--p2;
		}
		--p8;
		--p2;
	}
}