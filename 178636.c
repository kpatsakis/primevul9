jpeg_parse_exif_app1 (JpegExifContext *context, jpeg_saved_marker_ptr marker)
{
	guint i;
	guint ret = FALSE;
	guint offset;
	guint tags;	   /* number of tags in current ifd */
	guint endian = 0;	/* detected endian of data */
	const char leth[]  = {0x49, 0x49, 0x2a, 0x00};	// Little endian TIFF header
	const char beth[]  = {0x4d, 0x4d, 0x00, 0x2a};	// Big endian TIFF header

	/* do we have enough data? */
	if (marker->data_length < 4)
		goto out;

	/* unique identification string */
	if (memcmp (marker->data, "Exif", 4) != 0)
		goto out;

	/* do we have enough data? */
	if (marker->data_length < 32)
		goto out;

	/* Just skip data until TIFF header - it should be within 16 bytes from marker start.
	   Normal structure relative to APP1 marker -
		0x0000: APP1 marker entry = 2 bytes
		0x0002: APP1 length entry = 2 bytes
		0x0004: Exif Identifier entry = 6 bytes
		0x000A: Start of TIFF header (Byte order entry) - 4 bytes
			- This is what we look for, to determine endianess.
		0x000E: 0th IFD offset pointer - 4 bytes

		marker->data points to the first data after the APP1 marker
		and length entries, which is the exif identification string.
		The TIFF header should thus normally be found at i=6, below,
		and the pointer to IFD0 will be at 6+4 = 10.
	*/

	for (i=0; i<16; i++) {
		/* little endian TIFF header */
		if (memcmp (&marker->data[i], leth, 4) == 0) {
			endian = G_LITTLE_ENDIAN;
			ret = TRUE;
			break;
		}

		/* big endian TIFF header */
		if (memcmp (&marker->data[i], beth, 4) == 0) {
			endian = G_BIG_ENDIAN;
			ret = TRUE;
			break;
		}
	}

	/* could not find header */
	if (!ret)
		goto out;

	/* read out the offset pointer to IFD0 */
	offset  = de_get32(&marker->data[i] + 4, endian);
	i = i + offset;

	/* check that we still are within the buffer and can read the tag count */
	{
	    const size_t new_i = i + 2;
	    if (new_i < i || new_i > marker->data_length) {
		    ret = FALSE;
		    goto out;
	    }

	    /* find out how many tags we have in IFD0. As per the TIFF spec, the first
	       two bytes of the IFD contain a count of the number of tags. */
	    tags = de_get16(&marker->data[i], endian);
	    i = new_i;
	}

	/* check that we still have enough data for all tags to check. The tags
	   are listed in consecutive 12-byte blocks. The tag ID, type, size, and
	   a pointer to the actual value, are packed into these 12 byte entries. */
	{
	    const size_t new_i = i + tags * 12;
	    if (new_i < i || new_i > marker->data_length) {
		ret = FALSE;
		goto out;
	    }
	}

	/* check through IFD0 for tags */
	while (tags--) {
		size_t new_i;

		/* We check for integer overflow before the loop and
		 * at the end of each iteration */
		guint tag   = de_get16(&marker->data[i + 0], endian);
		guint type  = de_get16(&marker->data[i + 2], endian);
		guint count = de_get32(&marker->data[i + 4], endian);

		/* orientation tag? */
		if (tag == 0x112){

			/* The orientation field should consist of a single 2-byte integer,
			 * but might be a signed long.
			 * Values of types smaller than 4 bytes are stored directly in the
			 * Value Offset field */
			if (type == 0x3 && count == 1) {
				guint short_value = de_get16(&marker->data[i + 8], endian);

				context->orientation = short_value <= 8 ? short_value : 0;
			} else if (type == 0x9 && count == 1) {
				guint long_value = de_get32(&marker->data[i + 8], endian);

				context->orientation = long_value <= 8 ? long_value : 0;
			}
		}
		/* move the pointer to the next 12-byte tag field. */
		new_i = i + 12;
		if (new_i < i || new_i > marker->data_length) {
			ret = FALSE;
			goto out;
		}
		i = new_i;
	}

out:
	return ret;
}