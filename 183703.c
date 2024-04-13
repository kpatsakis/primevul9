FLAC_API FLAC__StreamEncoder *FLAC__stream_encoder_new(void)
{
	FLAC__StreamEncoder *encoder;
	uint32_t i;

	FLAC__ASSERT(sizeof(int) >= 4); /* we want to die right away if this is not true */

	encoder = calloc(1, sizeof(FLAC__StreamEncoder));
	if(encoder == 0) {
		return 0;
	}

	encoder->protected_ = calloc(1, sizeof(FLAC__StreamEncoderProtected));
	if(encoder->protected_ == 0) {
		free(encoder);
		return 0;
	}

	encoder->private_ = calloc(1, sizeof(FLAC__StreamEncoderPrivate));
	if(encoder->private_ == 0) {
		free(encoder->protected_);
		free(encoder);
		return 0;
	}

	encoder->private_->frame = FLAC__bitwriter_new();
	if(encoder->private_->frame == 0) {
		free(encoder->private_);
		free(encoder->protected_);
		free(encoder);
		return 0;
	}

	encoder->private_->file = 0;

	set_defaults_(encoder);

	encoder->private_->is_being_deleted = false;

	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		encoder->private_->subframe_workspace_ptr[i][0] = &encoder->private_->subframe_workspace[i][0];
		encoder->private_->subframe_workspace_ptr[i][1] = &encoder->private_->subframe_workspace[i][1];
	}
	for(i = 0; i < 2; i++) {
		encoder->private_->subframe_workspace_ptr_mid_side[i][0] = &encoder->private_->subframe_workspace_mid_side[i][0];
		encoder->private_->subframe_workspace_ptr_mid_side[i][1] = &encoder->private_->subframe_workspace_mid_side[i][1];
	}
	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		encoder->private_->partitioned_rice_contents_workspace_ptr[i][0] = &encoder->private_->partitioned_rice_contents_workspace[i][0];
		encoder->private_->partitioned_rice_contents_workspace_ptr[i][1] = &encoder->private_->partitioned_rice_contents_workspace[i][1];
	}
	for(i = 0; i < 2; i++) {
		encoder->private_->partitioned_rice_contents_workspace_ptr_mid_side[i][0] = &encoder->private_->partitioned_rice_contents_workspace_mid_side[i][0];
		encoder->private_->partitioned_rice_contents_workspace_ptr_mid_side[i][1] = &encoder->private_->partitioned_rice_contents_workspace_mid_side[i][1];
	}

	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		FLAC__format_entropy_coding_method_partitioned_rice_contents_init(&encoder->private_->partitioned_rice_contents_workspace[i][0]);
		FLAC__format_entropy_coding_method_partitioned_rice_contents_init(&encoder->private_->partitioned_rice_contents_workspace[i][1]);
	}
	for(i = 0; i < 2; i++) {
		FLAC__format_entropy_coding_method_partitioned_rice_contents_init(&encoder->private_->partitioned_rice_contents_workspace_mid_side[i][0]);
		FLAC__format_entropy_coding_method_partitioned_rice_contents_init(&encoder->private_->partitioned_rice_contents_workspace_mid_side[i][1]);
	}
	for(i = 0; i < 2; i++)
		FLAC__format_entropy_coding_method_partitioned_rice_contents_init(&encoder->private_->partitioned_rice_contents_extra[i]);

	encoder->protected_->state = FLAC__STREAM_ENCODER_UNINITIALIZED;

	return encoder;
}