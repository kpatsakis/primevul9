FLAC_API void FLAC__stream_encoder_delete(FLAC__StreamEncoder *encoder)
{
	uint32_t i;

	if (encoder == NULL)
		return ;

	FLAC__ASSERT(0 != encoder->protected_);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->private_->frame);

	encoder->private_->is_being_deleted = true;

	(void)FLAC__stream_encoder_finish(encoder);

	if(0 != encoder->private_->verify.decoder)
		FLAC__stream_decoder_delete(encoder->private_->verify.decoder);

	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		FLAC__format_entropy_coding_method_partitioned_rice_contents_clear(&encoder->private_->partitioned_rice_contents_workspace[i][0]);
		FLAC__format_entropy_coding_method_partitioned_rice_contents_clear(&encoder->private_->partitioned_rice_contents_workspace[i][1]);
	}
	for(i = 0; i < 2; i++) {
		FLAC__format_entropy_coding_method_partitioned_rice_contents_clear(&encoder->private_->partitioned_rice_contents_workspace_mid_side[i][0]);
		FLAC__format_entropy_coding_method_partitioned_rice_contents_clear(&encoder->private_->partitioned_rice_contents_workspace_mid_side[i][1]);
	}
	for(i = 0; i < 2; i++)
		FLAC__format_entropy_coding_method_partitioned_rice_contents_clear(&encoder->private_->partitioned_rice_contents_extra[i]);

	FLAC__bitwriter_delete(encoder->private_->frame);
	free(encoder->private_);
	free(encoder->protected_);
	free(encoder);
}