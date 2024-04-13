static int bmp_read_1bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0, current_byte = 0, bit = 0;

	if (info->enctype != BMP_BI_RGB) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 2;
	} else if (info->numcolors < 0 || info->numcolors > 2) {
		return 1;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	/* The line must be divisible by 4, else its padded with NULLs */
	padding = ((int)ceil(0.1 * info->width)) % 4;
	if (padding) {
		padding = 4 - padding;
	}

	for (ypos = 0; ypos < info->height; ++ypos) {
		if (info->topdown) {
			row = ypos;
		} else {
			row = info->height - ypos - 1;
		}

		for (xpos = 0; xpos < info->width; xpos += 8) {
			/* Bitmaps are always aligned in bytes so we'll never overflow */
			if (!gdGetByte(&current_byte, infile)) {
				return 1;
			}

			for (bit = 0; bit < 8; bit++) {
				index = ((current_byte & (0x80 >> bit)) != 0 ? 0x01 : 0x00);
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos + bit, row, index);
				/* No need to read anything extra */
				if ((xpos + bit) >= info->width) {
					break;
				}
			}
		}

		for (xpos = padding; xpos > 0; --xpos) {
			if (!gdGetByte(&index, infile)) {
				return 1;
			}
		}
	}
	return 0;
}