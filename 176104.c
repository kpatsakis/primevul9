void adv_png_expand_4(unsigned width, unsigned height, unsigned char* ptr)
{
	unsigned i, j;
	unsigned char* p8 = ptr + height * (width + 1) - 1;
	unsigned char* p4 = ptr + height * (width / 2 + 1) - 1;

	width /= 2;
	for(i=0;i<height;++i) {
		for(j=0;j<width;++j) {
			unsigned char v = *p4;
			*p8-- = v & 0xF;
			*p8-- = v >> 4;
			--p4;
		}
		--p8;
		--p4;
	}
}