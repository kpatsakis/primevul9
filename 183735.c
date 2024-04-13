void update_ogg_metadata_(FLAC__StreamEncoder *encoder)
{
	/* the # of bytes in the 1st packet that precede the STREAMINFO */
	static const uint32_t FIRST_OGG_PACKET_STREAMINFO_PREFIX_LENGTH =
		FLAC__OGG_MAPPING_PACKET_TYPE_LENGTH +
		FLAC__OGG_MAPPING_MAGIC_LENGTH +
		FLAC__OGG_MAPPING_VERSION_MAJOR_LENGTH +
		FLAC__OGG_MAPPING_VERSION_MINOR_LENGTH +
		FLAC__OGG_MAPPING_NUM_HEADERS_LENGTH +
		FLAC__STREAM_SYNC_LENGTH
	;
	FLAC__byte b[flac_max(6u, FLAC__STREAM_METADATA_SEEKPOINT_LENGTH)];
	const FLAC__StreamMetadata *metadata = &encoder->private_->streaminfo;
	const FLAC__uint64 samples = metadata->data.stream_info.total_samples;
	const uint32_t min_framesize = metadata->data.stream_info.min_framesize;
	const uint32_t max_framesize = metadata->data.stream_info.max_framesize;
	ogg_page page;

	FLAC__ASSERT(metadata->type == FLAC__METADATA_TYPE_STREAMINFO);
	FLAC__ASSERT(0 != encoder->private_->seek_callback);

	/* Pre-check that client supports seeking, since we don't want the
	 * ogg_helper code to ever have to deal with this condition.
	 */
	if(encoder->private_->seek_callback(encoder, 0, encoder->private_->client_data) == FLAC__STREAM_ENCODER_SEEK_STATUS_UNSUPPORTED)
		return;

	/* All this is based on intimate knowledge of the stream header
	 * layout, but a change to the header format that would break this
	 * would also break all streams encoded in the previous format.
	 */

	/**
	 ** Write STREAMINFO stats
	 **/
	simple_ogg_page__init(&page);
	if(!simple_ogg_page__get_at(encoder, encoder->protected_->streaminfo_offset, &page, encoder->private_->seek_callback, encoder->private_->read_callback, encoder->private_->client_data)) {
		simple_ogg_page__clear(&page);
		return; /* state already set */
	}

	/*
	 * Write MD5 signature
	 */
	{
		const uint32_t md5_offset =
			FIRST_OGG_PACKET_STREAMINFO_PREFIX_LENGTH +
			FLAC__STREAM_METADATA_HEADER_LENGTH +
			(
				FLAC__STREAM_METADATA_STREAMINFO_MIN_BLOCK_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_MAX_BLOCK_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_MIN_FRAME_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_MAX_FRAME_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_SAMPLE_RATE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_CHANNELS_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_BITS_PER_SAMPLE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_TOTAL_SAMPLES_LEN
			) / 8;

		if(md5_offset + 16 > (uint32_t)page.body_len) {
			encoder->protected_->state = FLAC__STREAM_ENCODER_OGG_ERROR;
			simple_ogg_page__clear(&page);
			return;
		}
		memcpy(page.body + md5_offset, metadata->data.stream_info.md5sum, 16);
	}

	/*
	 * Write total samples
	 */
	{
		const uint32_t total_samples_byte_offset =
			FIRST_OGG_PACKET_STREAMINFO_PREFIX_LENGTH +
			FLAC__STREAM_METADATA_HEADER_LENGTH +
			(
				FLAC__STREAM_METADATA_STREAMINFO_MIN_BLOCK_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_MAX_BLOCK_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_MIN_FRAME_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_MAX_FRAME_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_SAMPLE_RATE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_CHANNELS_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_BITS_PER_SAMPLE_LEN
				- 4
			) / 8;

		if(total_samples_byte_offset + 5 > (uint32_t)page.body_len) {
			encoder->protected_->state = FLAC__STREAM_ENCODER_OGG_ERROR;
			simple_ogg_page__clear(&page);
			return;
		}
		b[0] = (FLAC__byte)page.body[total_samples_byte_offset] & 0xF0;
		b[0] |= (FLAC__byte)((samples >> 32) & 0x0F);
		b[1] = (FLAC__byte)((samples >> 24) & 0xFF);
		b[2] = (FLAC__byte)((samples >> 16) & 0xFF);
		b[3] = (FLAC__byte)((samples >> 8) & 0xFF);
		b[4] = (FLAC__byte)(samples & 0xFF);
		memcpy(page.body + total_samples_byte_offset, b, 5);
	}

	/*
	 * Write min/max framesize
	 */
	{
		const uint32_t min_framesize_offset =
			FIRST_OGG_PACKET_STREAMINFO_PREFIX_LENGTH +
			FLAC__STREAM_METADATA_HEADER_LENGTH +
			(
				FLAC__STREAM_METADATA_STREAMINFO_MIN_BLOCK_SIZE_LEN +
				FLAC__STREAM_METADATA_STREAMINFO_MAX_BLOCK_SIZE_LEN
			) / 8;

		if(min_framesize_offset + 6 > (uint32_t)page.body_len) {
			encoder->protected_->state = FLAC__STREAM_ENCODER_OGG_ERROR;
			simple_ogg_page__clear(&page);
			return;
		}
		b[0] = (FLAC__byte)((min_framesize >> 16) & 0xFF);
		b[1] = (FLAC__byte)((min_framesize >> 8) & 0xFF);
		b[2] = (FLAC__byte)(min_framesize & 0xFF);
		b[3] = (FLAC__byte)((max_framesize >> 16) & 0xFF);
		b[4] = (FLAC__byte)((max_framesize >> 8) & 0xFF);
		b[5] = (FLAC__byte)(max_framesize & 0xFF);
		memcpy(page.body + min_framesize_offset, b, 6);
	}
	if(!simple_ogg_page__set_at(encoder, encoder->protected_->streaminfo_offset, &page, encoder->private_->seek_callback, encoder->private_->write_callback, encoder->private_->client_data)) {
		simple_ogg_page__clear(&page);
		return; /* state already set */
	}
	simple_ogg_page__clear(&page);

	/*
	 * Write seektable
	 */
	if(0 != encoder->private_->seek_table && encoder->private_->seek_table->num_points > 0 && encoder->protected_->seektable_offset > 0) {
		uint32_t i;
		FLAC__byte *p;

		FLAC__format_seektable_sort(encoder->private_->seek_table);

		FLAC__ASSERT(FLAC__format_seektable_is_legal(encoder->private_->seek_table));

		simple_ogg_page__init(&page);
		if(!simple_ogg_page__get_at(encoder, encoder->protected_->seektable_offset, &page, encoder->private_->seek_callback, encoder->private_->read_callback, encoder->private_->client_data)) {
			simple_ogg_page__clear(&page);
			return; /* state already set */
		}

		if((FLAC__STREAM_METADATA_HEADER_LENGTH + 18*encoder->private_->seek_table->num_points) != (uint32_t)page.body_len) {
			encoder->protected_->state = FLAC__STREAM_ENCODER_OGG_ERROR;
			simple_ogg_page__clear(&page);
			return;
		}

		for(i = 0, p = page.body + FLAC__STREAM_METADATA_HEADER_LENGTH; i < encoder->private_->seek_table->num_points; i++, p += 18) {
			FLAC__uint64 xx;
			uint32_t x;
			xx = encoder->private_->seek_table->points[i].sample_number;
			b[7] = (FLAC__byte)xx; xx >>= 8;
			b[6] = (FLAC__byte)xx; xx >>= 8;
			b[5] = (FLAC__byte)xx; xx >>= 8;
			b[4] = (FLAC__byte)xx; xx >>= 8;
			b[3] = (FLAC__byte)xx; xx >>= 8;
			b[2] = (FLAC__byte)xx; xx >>= 8;
			b[1] = (FLAC__byte)xx; xx >>= 8;
			b[0] = (FLAC__byte)xx; xx >>= 8;
			xx = encoder->private_->seek_table->points[i].stream_offset;
			b[15] = (FLAC__byte)xx; xx >>= 8;
			b[14] = (FLAC__byte)xx; xx >>= 8;
			b[13] = (FLAC__byte)xx; xx >>= 8;
			b[12] = (FLAC__byte)xx; xx >>= 8;
			b[11] = (FLAC__byte)xx; xx >>= 8;
			b[10] = (FLAC__byte)xx; xx >>= 8;
			b[9] = (FLAC__byte)xx; xx >>= 8;
			b[8] = (FLAC__byte)xx; xx >>= 8;
			x = encoder->private_->seek_table->points[i].frame_samples;
			b[17] = (FLAC__byte)x; x >>= 8;
			b[16] = (FLAC__byte)x; x >>= 8;
			memcpy(p, b, 18);
		}

		if(!simple_ogg_page__set_at(encoder, encoder->protected_->seektable_offset, &page, encoder->private_->seek_callback, encoder->private_->write_callback, encoder->private_->client_data)) {
			simple_ogg_page__clear(&page);
			return; /* state already set */
		}
		simple_ogg_page__clear(&page);
	}
}