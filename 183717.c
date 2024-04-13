void set_defaults_(FLAC__StreamEncoder *encoder)
{
	FLAC__ASSERT(0 != encoder);

#ifdef FLAC__MANDATORY_VERIFY_WHILE_ENCODING
	encoder->protected_->verify = true;
#else
	encoder->protected_->verify = false;
#endif
	encoder->protected_->streamable_subset = true;
	encoder->protected_->do_md5 = true;
	encoder->protected_->do_mid_side_stereo = false;
	encoder->protected_->loose_mid_side_stereo = false;
	encoder->protected_->channels = 2;
	encoder->protected_->bits_per_sample = 16;
	encoder->protected_->sample_rate = 44100;
	encoder->protected_->blocksize = 0;
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	encoder->protected_->num_apodizations = 1;
	encoder->protected_->apodizations[0].type = FLAC__APODIZATION_TUKEY;
	encoder->protected_->apodizations[0].parameters.tukey.p = 0.5;
#endif
	encoder->protected_->max_lpc_order = 0;
	encoder->protected_->qlp_coeff_precision = 0;
	encoder->protected_->do_qlp_coeff_prec_search = false;
	encoder->protected_->do_exhaustive_model_search = false;
	encoder->protected_->do_escape_coding = false;
	encoder->protected_->min_residual_partition_order = 0;
	encoder->protected_->max_residual_partition_order = 0;
	encoder->protected_->rice_parameter_search_dist = 0;
	encoder->protected_->total_samples_estimate = 0;
	encoder->protected_->metadata = 0;
	encoder->protected_->num_metadata_blocks = 0;

	encoder->private_->seek_table = 0;
	encoder->private_->disable_constant_subframes = false;
	encoder->private_->disable_fixed_subframes = false;
	encoder->private_->disable_verbatim_subframes = false;
	encoder->private_->is_ogg = false;
	encoder->private_->read_callback = 0;
	encoder->private_->write_callback = 0;
	encoder->private_->seek_callback = 0;
	encoder->private_->tell_callback = 0;
	encoder->private_->metadata_callback = 0;
	encoder->private_->progress_callback = 0;
	encoder->private_->client_data = 0;

#if FLAC__HAS_OGG
	FLAC__ogg_encoder_aspect_set_defaults(&encoder->protected_->ogg_encoder_aspect);
#endif

	FLAC__stream_encoder_set_compression_level(encoder, 5);
}