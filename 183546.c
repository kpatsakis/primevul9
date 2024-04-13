static int bmp_read_4bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0, current_byte = 0;

	if (info->enctype != BMP_BI_RGB && info->enctype != BMP_BI_RLE4) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 16;
	} else if (info->numcolors < 0 || info->numcolors > 16) {
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
	padding = ((int)ceil(0.5 * info->width)) % 4;
	if (padding) {
		padding = 4 - padding;
	}

	switch (info->enctype) {
	case BMP_BI_RGB:
		for (ypos = 0; ypos < info->height; ++ypos) {
			if (info->topdown) {
				row = ypos;
			} else {
				row = info->height - ypos - 1;
			}

			for (xpos = 0; xpos < info->width; xpos += 2) {
				if (!gdGetByte(&current_byte, infile)) {
					return 1;
				}

				index = (current_byte >> 4) & 0x0f;
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos, row, index);

				/* This condition may get called often, potential optimsations */
				if (xpos >= info->width) {
					break;
				}

				index = current_byte & 0x0f;
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos + 1, row, index);
			}

			for (xpos = padding; xpos > 0; --xpos) {
				if (!gdGetByte(&index, infile)) {
					return 1;
				}
			}
		}
		break;

	case BMP_BI_RLE4:
		if (bmp_read_rle(im, infile, info)) {
			return 1;
		}
		break;

	default:
		return 1;
	}
	return 0;
}