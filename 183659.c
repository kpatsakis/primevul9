FLAC_API FLAC__bool FLAC__stream_encoder_set_metadata(FLAC__StreamEncoder *encoder, FLAC__StreamMetadata **metadata, uint32_t num_blocks)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	if(0 == metadata)
		num_blocks = 0;
	if(0 == num_blocks)
		metadata = 0;
	/* realloc() does not do exactly what we want so... */
	if(encoder->protected_->metadata) {
		free(encoder->protected_->metadata);
		encoder->protected_->metadata = 0;
		encoder->protected_->num_metadata_blocks = 0;
	}
	if(num_blocks) {
		FLAC__StreamMetadata **m;
		if(0 == (m = safe_malloc_mul_2op_p(sizeof(m[0]), /*times*/num_blocks)))
			return false;
		memcpy(m, metadata, sizeof(m[0]) * num_blocks);
		encoder->protected_->metadata = m;
		encoder->protected_->num_metadata_blocks = num_blocks;
	}
#if FLAC__HAS_OGG
	if(!FLAC__ogg_encoder_aspect_set_num_metadata(&encoder->protected_->ogg_encoder_aspect, num_blocks))
		return false;
#endif
	return true;
}