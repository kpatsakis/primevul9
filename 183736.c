FLAC_API FLAC__bool FLAC__stream_encoder_set_compression_level(FLAC__StreamEncoder *encoder, uint32_t value)
{
	FLAC__bool ok = true;
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	if(value >= sizeof(compression_levels_)/sizeof(compression_levels_[0]))
		value = sizeof(compression_levels_)/sizeof(compression_levels_[0]) - 1;
	ok &= FLAC__stream_encoder_set_do_mid_side_stereo          (encoder, compression_levels_[value].do_mid_side_stereo);
	ok &= FLAC__stream_encoder_set_loose_mid_side_stereo       (encoder, compression_levels_[value].loose_mid_side_stereo);
#ifndef FLAC__INTEGER_ONLY_LIBRARY
#if 1
	ok &= FLAC__stream_encoder_set_apodization                 (encoder, compression_levels_[value].apodization);
#else
	/* equivalent to -A tukey(0.5) */
	encoder->protected_->num_apodizations = 1;
	encoder->protected_->apodizations[0].type = FLAC__APODIZATION_TUKEY;
	encoder->protected_->apodizations[0].parameters.tukey.p = 0.5;
#endif
#endif
	ok &= FLAC__stream_encoder_set_max_lpc_order               (encoder, compression_levels_[value].max_lpc_order);
	ok &= FLAC__stream_encoder_set_qlp_coeff_precision         (encoder, compression_levels_[value].qlp_coeff_precision);
	ok &= FLAC__stream_encoder_set_do_qlp_coeff_prec_search    (encoder, compression_levels_[value].do_qlp_coeff_prec_search);
	ok &= FLAC__stream_encoder_set_do_escape_coding            (encoder, compression_levels_[value].do_escape_coding);
	ok &= FLAC__stream_encoder_set_do_exhaustive_model_search  (encoder, compression_levels_[value].do_exhaustive_model_search);
	ok &= FLAC__stream_encoder_set_min_residual_partition_order(encoder, compression_levels_[value].min_residual_partition_order);
	ok &= FLAC__stream_encoder_set_max_residual_partition_order(encoder, compression_levels_[value].max_residual_partition_order);
	ok &= FLAC__stream_encoder_set_rice_parameter_search_dist  (encoder, compression_levels_[value].rice_parameter_search_dist);
	return ok;
}