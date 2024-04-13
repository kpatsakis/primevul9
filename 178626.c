jpeg_parse_exif_app2_segment (JpegExifContext *context, jpeg_saved_marker_ptr marker)
{
	guint ret = FALSE;
	guint sequence_number;
	guint number_of_chunks;
	guint chunk_size;
	guint offset;

	/* do we have enough data? */
	if (marker->data_length < 16)
		goto out;

	/* unique identification string */
	if (memcmp (marker->data, "ICC_PROFILE\0", 12) != 0)
		goto out;

	/* get data about this segment */
	sequence_number = marker->data[12];
	number_of_chunks = marker->data[13];

	/* this is invalid, the base offset is 1 */
	if (sequence_number == 0)
		goto out;

	/* this is invalid, the base offset is 1 */
	if (sequence_number > number_of_chunks)
		goto out;

	/* size includes the id (12 bytes), length field (1 byte), and sequence field (1 byte) */
	chunk_size = marker->data_length - 14;
	offset = (sequence_number - 1) * 0xffef;

	/* Deal with the trivial profile (99% of images) to avoid allocating
	 * 64kb when we might only use a few kb. */
	if (number_of_chunks == 1) {
		if (context->icc_profile_size_allocated > 0)
			goto out;
		context->icc_profile_size = chunk_size;
		context->icc_profile_size_allocated = chunk_size;
		context->icc_profile = g_new (gchar, chunk_size);
		/* copy the segment data to the profile space */
		memcpy (context->icc_profile, marker->data + 14, chunk_size);
		goto out;
	}

	/* There is no promise the APP2 segments are going to be in order, so we
	 * have to allocate a huge swathe of memory and fill in the gaps when
	 * (if) we get the segment.
	 * Theoretically this could be as much as 16Mb, but display profiles are
	 * vary rarely above 100kb, and printer profiles are usually less than
	 * 2Mb */
	if (context->icc_profile_size_allocated == 0) {
		context->icc_profile_size_allocated = number_of_chunks * 0xffff;
		context->icc_profile = g_new0 (gchar, number_of_chunks * 0xffff);
	}

	/* check the data will fit in our previously allocated buffer */
	if (offset + chunk_size > context->icc_profile_size_allocated)
		goto out;

	/* copy the segment data to the profile space */
	memcpy (context->icc_profile + offset, marker->data + 14, chunk_size);

	/* it's now this big plus the new data we've just copied */
	context->icc_profile_size += chunk_size;

	/* success */
	ret = TRUE;
out:
	return ret;
}