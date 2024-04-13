BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx (gdIOCtx * infile)
{
	png_byte sig[8];
#ifdef PNG_SETJMP_SUPPORTED
	jmpbuf_wrapper jbw;
#endif
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height, rowbytes, w, h, res_x, res_y;
	int bit_depth, color_type, interlace_type, unit_type;
	int num_palette = 0, num_trans;
	png_colorp palette;
	png_color_16p trans_gray_rgb;
	png_color_16p trans_color_rgb;
	png_bytep trans;
	png_bytep image_data = NULL;
	png_bytepp row_pointers = NULL;
	gdImagePtr im = NULL;
	int i, j, *open = NULL;
	volatile int transparent = -1;
	volatile int palette_allocated = FALSE;

	/* Make sure the signature can't match by dumb luck -- TBB */
	/* GRR: isn't sizeof(infile) equal to the size of the pointer? */
	memset (sig, 0, sizeof (sig));

	/* first do a quick check that the file really is a PNG image; could
	 * have used slightly more general png_sig_cmp() function instead */
	if (gdGetBuf (sig, 8, infile) < 8) {
		return NULL;
	}

	if (png_sig_cmp(sig, 0, 8) != 0) { /* bad signature */
		return NULL;		/* bad signature */
	}

#ifdef PNG_SETJMP_SUPPORTED
	png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, &jbw, gdPngErrorHandler, NULL);
#else
	png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#endif
	if (png_ptr == NULL) {
		gd_error("gd-png error: cannot allocate libpng main struct\n");
		return NULL;
	}

	info_ptr = png_create_info_struct (png_ptr);
	if (info_ptr == NULL) {
		gd_error("gd-png error: cannot allocate libpng info struct\n");
		png_destroy_read_struct (&png_ptr, NULL, NULL);

		return NULL;
	}

	/* we could create a second info struct here (end_info), but it's only
	 * useful if we want to keep pre- and post-IDAT chunk info separated
	 * (mainly for PNG-aware image editors and converters)
	 */

	/* setjmp() must be called in every non-callback function that calls a
	 * PNG-reading libpng function.  We must reset it everytime we get a
	 * new allocation that we save in a stack variable.
	 */
#ifdef PNG_SETJMP_SUPPORTED
	if (setjmp(jbw.jmpbuf)) {
		gd_error("gd-png error: setjmp returns error condition 1\n");
		png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

		return NULL;
	}
#endif

	png_set_sig_bytes (png_ptr, 8);	/* we already read the 8 signature bytes */

	png_set_read_fn (png_ptr, (void *) infile, gdPngReadData);
	png_read_info (png_ptr, info_ptr);	/* read all PNG info up to image data */

	png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	if ((color_type == PNG_COLOR_TYPE_RGB) || (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	        || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		im = gdImageCreateTrueColor ((int) width, (int) height);
	} else {
		im = gdImageCreate ((int) width, (int) height);
	}
	if (im == NULL) {
		gd_error("gd-png error: cannot allocate gdImage struct\n");
		goto error;
	}

	if (bit_depth == 16) {
		png_set_strip_16 (png_ptr);
	} else if (bit_depth < 8) {
		png_set_packing (png_ptr);	/* expand to 1 byte per pixel */
	}

	/* setjmp() must be called in every non-callback function that calls a
	 * PNG-reading libpng function.  We must reset it everytime we get a
	 * new allocation that we save in a stack variable.
	 */
#ifdef PNG_SETJMP_SUPPORTED
	if (setjmp(jbw.jmpbuf)) {
		gd_error("gd-png error: setjmp returns error condition 2\n");
		goto error;
	}
#endif

#ifdef PNG_pHYs_SUPPORTED
	/* check if the resolution is specified */
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs)) {
		if (png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type)) {
			switch (unit_type) {
			case PNG_RESOLUTION_METER:
				im->res_x = DPM2DPI(res_x);
				im->res_y = DPM2DPI(res_y);
				break;
			}
		}
	}
#endif

	switch (color_type) {
	case PNG_COLOR_TYPE_PALETTE:
		png_get_PLTE (png_ptr, info_ptr, &palette, &num_palette);
#ifdef DEBUG
		gd_error("gd-png color_type is palette, colors: %d\n", num_palette);
#endif /* DEBUG */
		if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS)) {
			/* gd 2.0: we support this rather thoroughly now. Grab the
			 * first fully transparent entry, if any, as the value of
			 * the simple-transparency index, mostly for backwards
			 * binary compatibility. The alpha channel is where it's
			 * really at these days.
			 */
			int firstZero = 1;
			png_get_tRNS (png_ptr, info_ptr, &trans, &num_trans, NULL);
			for (i = 0; i < num_trans; ++i) {
				im->alpha[i] = gdAlphaMax - (trans[i] >> 1);
				if ((trans[i] == 0) && (firstZero)) {
					/* 2.0.5: long-forgotten patch from Wez Furlong */
					transparent = i;
					firstZero = 0;
				}
			}
		}
		break;

	case PNG_COLOR_TYPE_GRAY:
		/* create a fake palette and check for single-shade transparency */
		if ((palette = (png_colorp) gdMalloc (256 * sizeof (png_color))) == NULL) {
			gd_error("gd-png error: cannot allocate gray palette\n");
			goto error;
		}
		palette_allocated = TRUE;
		if (bit_depth < 8) {
			num_palette = 1 << bit_depth;
			for (i = 0; i < 256; ++i) {
				j = (255 * i) / (num_palette - 1);
				palette[i].red = palette[i].green = palette[i].blue = j;
			}
		} else {
			num_palette = 256;
			for (i = 0; i < 256; ++i) {
				palette[i].red = palette[i].green = palette[i].blue = i;
			}
		}
		if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_get_tRNS (png_ptr, info_ptr, NULL, NULL, &trans_gray_rgb);
			if (bit_depth == 16) {	/* png_set_strip_16() not yet in effect */
				transparent = trans_gray_rgb->gray >> 8;
			} else {
				transparent = trans_gray_rgb->gray;
			}
			/* Note slight error in 16-bit case:  up to 256 16-bit shades
			 * may get mapped to a single 8-bit shade, and only one of them
			 * is supposed to be transparent.  IOW, both opaque pixels and
			 * transparent pixels will be mapped into the transparent entry.
			 * There is no particularly good way around this in the case
			 * that all 256 8-bit shades are used, but one could write some
			 * custom 16-bit code to handle the case where there are gdFree
			 * palette entries.  This error will be extremely rare in
			 * general, though.  (Quite possibly there is only one such
			 * image in existence.) */
		}
		break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
		png_set_gray_to_rgb(png_ptr);

	case PNG_COLOR_TYPE_RGB:
	case PNG_COLOR_TYPE_RGB_ALPHA:
		/* gd 2.0: we now support truecolor. See the comment above
		   for a rare situation in which the transparent pixel may not
		   work properly with 16-bit channels. */
		if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_get_tRNS (png_ptr, info_ptr, NULL, NULL, &trans_color_rgb);
			if (bit_depth == 16) {	/* png_set_strip_16() not yet in effect */
				transparent = gdTrueColor (trans_color_rgb->red >> 8,
				                           trans_color_rgb->green >> 8,
				                           trans_color_rgb->blue >> 8);
			} else {
				transparent = gdTrueColor (trans_color_rgb->red,
				                           trans_color_rgb->green,
				                           trans_color_rgb->blue);
			}
		}
		break;
	default:
		gd_error("gd-png color_type is unknown: %d\n", color_type);
		goto error;
	}

	png_read_update_info (png_ptr, info_ptr);

	/* allocate space for the PNG image data */
	rowbytes = png_get_rowbytes (png_ptr, info_ptr);
	if (overflow2(rowbytes, height))
		goto error;
	image_data = (png_bytep) gdMalloc (rowbytes * height);
	if (!image_data) {
		gd_error("gd-png error: cannot allocate image data\n");
		goto error;
	}
	if (overflow2(height, sizeof (png_bytep)))
		goto error;

	row_pointers = (png_bytepp) gdMalloc (height * sizeof (png_bytep));
	if (!row_pointers) {
		gd_error("gd-png error: cannot allocate row pointers\n");
		goto error;
	}

	/* setjmp() must be called in every non-callback function that calls a
	 * PNG-reading libpng function.  We must reset it everytime we get a
	 * new allocation that we save in a stack variable.
	 */
#ifdef PNG_SETJMP_SUPPORTED
	if (setjmp(jbw.jmpbuf)) {
		gd_error("gd-png error: setjmp returns error condition 3\n");
		goto error;
	}
#endif

	/* set the individual row_pointers to point at the correct offsets */
	for (h = 0; h < height; ++h) {
		row_pointers[h] = image_data + h * rowbytes;
	}

	png_read_image (png_ptr, row_pointers);	/* read whole image... */
	png_read_end (png_ptr, NULL);	/* ...done! */

	if (!im->trueColor) {
		im->colorsTotal = num_palette;
		/* load the palette and mark all entries "open" (unused) for now */
		open = im->open;
		for (i = 0; i < num_palette; ++i) {
			im->red[i] = palette[i].red;
			im->green[i] = palette[i].green;
			im->blue[i] = palette[i].blue;
			open[i] = 1;
		}
		for (i = num_palette; i < gdMaxColors; ++i) {
			open[i] = 1;
		}
	}
	/* 2.0.12: Slaven Rezic: palette images are not the only images
	   with a simple transparent color setting */
	im->transparent = transparent;
	im->interlace = (interlace_type == PNG_INTERLACE_ADAM7);

	/* can't nuke structs until done with palette */
	png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
	switch (color_type) {
	case PNG_COLOR_TYPE_RGB:
		for (h = 0; h < height; h++) {
			int boffset = 0;
			for (w = 0; w < width; w++) {
				register png_byte r = row_pointers[h][boffset++];
				register png_byte g = row_pointers[h][boffset++];
				register png_byte b = row_pointers[h][boffset++];
				im->tpixels[h][w] = gdTrueColor (r, g, b);
			}
		}
		break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
	case PNG_COLOR_TYPE_RGB_ALPHA:
		for (h = 0; h < height; h++) {
			int boffset = 0;
			for (w = 0; w < width; w++) {
				register png_byte r = row_pointers[h][boffset++];
				register png_byte g = row_pointers[h][boffset++];
				register png_byte b = row_pointers[h][boffset++];

				/* gd has only 7 bits of alpha channel resolution, and
				 * 127 is transparent, 0 opaque. A moment of convenience,
				 *  a lifetime of compatibility.
				 */

				register png_byte a = gdAlphaMax - (row_pointers[h][boffset++] >> 1);
				im->tpixels[h][w] = gdTrueColorAlpha(r, g, b, a);
			}
		}
		break;
	default:
		if (!im->trueColor) {
			/* Palette image, or something coerced to be one */
			for (h = 0; h < height; ++h) {
				for (w = 0; w < width; ++w) {
					register png_byte idx = row_pointers[h][w];
					im->pixels[h][w] = idx;
					open[idx] = 0;
				}
			}
		}
	}
#ifdef DEBUG
	if (!im->trueColor) {
		for (i = num_palette; i < gdMaxColors; ++i) {
			if (!open[i]) {
				fprintf (stderr,
				         "gd-png warning: image data references out-of-range"
				         " color index (%d)\n", i);
			}
		}
	}
#endif

 done:
	if (palette_allocated) {
		gdFree (palette);
	}
	if (image_data)
		gdFree(image_data);
	if (row_pointers)
		gdFree(row_pointers);

	return im;

 error:
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	if (im) {
		gdImageDestroy(im);
		im = NULL;
	}
	goto done;
}