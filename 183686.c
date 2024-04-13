static FLAC__StreamEncoderInitStatus init_stream_internal_(
	FLAC__StreamEncoder *encoder,
	FLAC__StreamEncoderReadCallback read_callback,
	FLAC__StreamEncoderWriteCallback write_callback,
	FLAC__StreamEncoderSeekCallback seek_callback,
	FLAC__StreamEncoderTellCallback tell_callback,
	FLAC__StreamEncoderMetadataCallback metadata_callback,
	void *client_data,
	FLAC__bool is_ogg
)
{
	uint32_t i;
	FLAC__bool metadata_has_seektable, metadata_has_vorbis_comment, metadata_picture_has_type1, metadata_picture_has_type2;

	FLAC__ASSERT(0 != encoder);

	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return FLAC__STREAM_ENCODER_INIT_STATUS_ALREADY_INITIALIZED;

	if(FLAC__HAS_OGG == 0 && is_ogg)
		return FLAC__STREAM_ENCODER_INIT_STATUS_UNSUPPORTED_CONTAINER;

	if(0 == write_callback || (seek_callback && 0 == tell_callback))
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_CALLBACKS;

	if(encoder->protected_->channels == 0 || encoder->protected_->channels > FLAC__MAX_CHANNELS)
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_NUMBER_OF_CHANNELS;

	if(encoder->protected_->channels != 2) {
		encoder->protected_->do_mid_side_stereo = false;
		encoder->protected_->loose_mid_side_stereo = false;
	}
	else if(!encoder->protected_->do_mid_side_stereo)
		encoder->protected_->loose_mid_side_stereo = false;

	if(encoder->protected_->bits_per_sample >= 32)
		encoder->protected_->do_mid_side_stereo = false; /* since we currently do 32-bit math, the side channel would have 33 bps and overflow */

	if(encoder->protected_->bits_per_sample < FLAC__MIN_BITS_PER_SAMPLE || encoder->protected_->bits_per_sample > FLAC__REFERENCE_CODEC_MAX_BITS_PER_SAMPLE)
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BITS_PER_SAMPLE;

	if(!FLAC__format_sample_rate_is_valid(encoder->protected_->sample_rate))
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_SAMPLE_RATE;

	if(encoder->protected_->blocksize == 0) {
		if(encoder->protected_->max_lpc_order == 0)
			encoder->protected_->blocksize = 1152;
		else
			encoder->protected_->blocksize = 4096;
	}

	if(encoder->protected_->blocksize < FLAC__MIN_BLOCK_SIZE || encoder->protected_->blocksize > FLAC__MAX_BLOCK_SIZE)
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BLOCK_SIZE;

	if(encoder->protected_->max_lpc_order > FLAC__MAX_LPC_ORDER)
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_MAX_LPC_ORDER;

	if(encoder->protected_->blocksize < encoder->protected_->max_lpc_order)
		return FLAC__STREAM_ENCODER_INIT_STATUS_BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER;

	if(encoder->protected_->qlp_coeff_precision == 0) {
		if(encoder->protected_->bits_per_sample < 16) {
			/* @@@ need some data about how to set this here w.r.t. blocksize and sample rate */
			/* @@@ until then we'll make a guess */
			encoder->protected_->qlp_coeff_precision = flac_max(FLAC__MIN_QLP_COEFF_PRECISION, 2 + encoder->protected_->bits_per_sample / 2);
		}
		else if(encoder->protected_->bits_per_sample == 16) {
			if(encoder->protected_->blocksize <= 192)
				encoder->protected_->qlp_coeff_precision = 7;
			else if(encoder->protected_->blocksize <= 384)
				encoder->protected_->qlp_coeff_precision = 8;
			else if(encoder->protected_->blocksize <= 576)
				encoder->protected_->qlp_coeff_precision = 9;
			else if(encoder->protected_->blocksize <= 1152)
				encoder->protected_->qlp_coeff_precision = 10;
			else if(encoder->protected_->blocksize <= 2304)
				encoder->protected_->qlp_coeff_precision = 11;
			else if(encoder->protected_->blocksize <= 4608)
				encoder->protected_->qlp_coeff_precision = 12;
			else
				encoder->protected_->qlp_coeff_precision = 13;
		}
		else {
			if(encoder->protected_->blocksize <= 384)
				encoder->protected_->qlp_coeff_precision = FLAC__MAX_QLP_COEFF_PRECISION-2;
			else if(encoder->protected_->blocksize <= 1152)
				encoder->protected_->qlp_coeff_precision = FLAC__MAX_QLP_COEFF_PRECISION-1;
			else
				encoder->protected_->qlp_coeff_precision = FLAC__MAX_QLP_COEFF_PRECISION;
		}
		FLAC__ASSERT(encoder->protected_->qlp_coeff_precision <= FLAC__MAX_QLP_COEFF_PRECISION);
	}
	else if(encoder->protected_->qlp_coeff_precision < FLAC__MIN_QLP_COEFF_PRECISION || encoder->protected_->qlp_coeff_precision > FLAC__MAX_QLP_COEFF_PRECISION)
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_QLP_COEFF_PRECISION;

	if(encoder->protected_->streamable_subset) {
		if(!FLAC__format_blocksize_is_subset(encoder->protected_->blocksize, encoder->protected_->sample_rate))
			return FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE;
		if(!FLAC__format_sample_rate_is_subset(encoder->protected_->sample_rate))
			return FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE;
		if(
			encoder->protected_->bits_per_sample != 8 &&
			encoder->protected_->bits_per_sample != 12 &&
			encoder->protected_->bits_per_sample != 16 &&
			encoder->protected_->bits_per_sample != 20 &&
			encoder->protected_->bits_per_sample != 24
		)
			return FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE;
		if(encoder->protected_->max_residual_partition_order > FLAC__SUBSET_MAX_RICE_PARTITION_ORDER)
			return FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE;
		if(
			encoder->protected_->sample_rate <= 48000 &&
			(
				encoder->protected_->blocksize > FLAC__SUBSET_MAX_BLOCK_SIZE_48000HZ ||
				encoder->protected_->max_lpc_order > FLAC__SUBSET_MAX_LPC_ORDER_48000HZ
			)
		) {
			return FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE;
		}
	}

	if(encoder->protected_->max_residual_partition_order >= (1u << FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ORDER_LEN))
		encoder->protected_->max_residual_partition_order = (1u << FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ORDER_LEN) - 1;
	if(encoder->protected_->min_residual_partition_order >= encoder->protected_->max_residual_partition_order)
		encoder->protected_->min_residual_partition_order = encoder->protected_->max_residual_partition_order;

#if FLAC__HAS_OGG
	/* reorder metadata if necessary to ensure that any VORBIS_COMMENT is the first, according to the mapping spec */
	if(is_ogg && 0 != encoder->protected_->metadata && encoder->protected_->num_metadata_blocks > 1) {
		uint32_t i1;
		for(i1 = 1; i1 < encoder->protected_->num_metadata_blocks; i1++) {
			if(0 != encoder->protected_->metadata[i1] && encoder->protected_->metadata[i1]->type == FLAC__METADATA_TYPE_VORBIS_COMMENT) {
				FLAC__StreamMetadata *vc = encoder->protected_->metadata[i1];
				for( ; i1 > 0; i1--)
					encoder->protected_->metadata[i1] = encoder->protected_->metadata[i1-1];
				encoder->protected_->metadata[0] = vc;
				break;
			}
		}
	}
#endif
	/* keep track of any SEEKTABLE block */
	if(0 != encoder->protected_->metadata && encoder->protected_->num_metadata_blocks > 0) {
		uint32_t i2;
		for(i2 = 0; i2 < encoder->protected_->num_metadata_blocks; i2++) {
			if(0 != encoder->protected_->metadata[i2] && encoder->protected_->metadata[i2]->type == FLAC__METADATA_TYPE_SEEKTABLE) {
				encoder->private_->seek_table = &encoder->protected_->metadata[i2]->data.seek_table;
				break; /* take only the first one */
			}
		}
	}

	/* validate metadata */
	if(0 == encoder->protected_->metadata && encoder->protected_->num_metadata_blocks > 0)
		return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
	metadata_has_seektable = false;
	metadata_has_vorbis_comment = false;
	metadata_picture_has_type1 = false;
	metadata_picture_has_type2 = false;
	for(i = 0; i < encoder->protected_->num_metadata_blocks; i++) {
		const FLAC__StreamMetadata *m = encoder->protected_->metadata[i];
		if(m->type == FLAC__METADATA_TYPE_STREAMINFO)
			return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
		else if(m->type == FLAC__METADATA_TYPE_SEEKTABLE) {
			if(metadata_has_seektable) /* only one is allowed */
				return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
			metadata_has_seektable = true;
			if(!FLAC__format_seektable_is_legal(&m->data.seek_table))
				return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
		}
		else if(m->type == FLAC__METADATA_TYPE_VORBIS_COMMENT) {
			if(metadata_has_vorbis_comment) /* only one is allowed */
				return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
			metadata_has_vorbis_comment = true;
		}
		else if(m->type == FLAC__METADATA_TYPE_CUESHEET) {
			if(!FLAC__format_cuesheet_is_legal(&m->data.cue_sheet, m->data.cue_sheet.is_cd, /*violation=*/0))
				return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
		}
		else if(m->type == FLAC__METADATA_TYPE_PICTURE) {
			if(!FLAC__format_picture_is_legal(&m->data.picture, /*violation=*/0))
				return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
			if(m->data.picture.type == FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON_STANDARD) {
				if(metadata_picture_has_type1) /* there should only be 1 per stream */
					return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
				metadata_picture_has_type1 = true;
				/* standard icon must be 32x32 pixel PNG */
				if(
					m->data.picture.type == FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON_STANDARD &&
					(
						(strcmp(m->data.picture.mime_type, "image/png") && strcmp(m->data.picture.mime_type, "-->")) ||
						m->data.picture.width != 32 ||
						m->data.picture.height != 32
					)
				)
					return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
			}
			else if(m->data.picture.type == FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON) {
				if(metadata_picture_has_type2) /* there should only be 1 per stream */
					return FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA;
				metadata_picture_has_type2 = true;
			}
		}
	}

	encoder->private_->input_capacity = 0;
	for(i = 0; i < encoder->protected_->channels; i++) {
		encoder->private_->integer_signal_unaligned[i] = encoder->private_->integer_signal[i] = 0;
#ifndef FLAC__INTEGER_ONLY_LIBRARY
		encoder->private_->real_signal_unaligned[i] = encoder->private_->real_signal[i] = 0;
#endif
	}
	for(i = 0; i < 2; i++) {
		encoder->private_->integer_signal_mid_side_unaligned[i] = encoder->private_->integer_signal_mid_side[i] = 0;
#ifndef FLAC__INTEGER_ONLY_LIBRARY
		encoder->private_->real_signal_mid_side_unaligned[i] = encoder->private_->real_signal_mid_side[i] = 0;
#endif
	}
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	for(i = 0; i < encoder->protected_->num_apodizations; i++)
		encoder->private_->window_unaligned[i] = encoder->private_->window[i] = 0;
	encoder->private_->windowed_signal_unaligned = encoder->private_->windowed_signal = 0;
#endif
	for(i = 0; i < encoder->protected_->channels; i++) {
		encoder->private_->residual_workspace_unaligned[i][0] = encoder->private_->residual_workspace[i][0] = 0;
		encoder->private_->residual_workspace_unaligned[i][1] = encoder->private_->residual_workspace[i][1] = 0;
		encoder->private_->best_subframe[i] = 0;
	}
	for(i = 0; i < 2; i++) {
		encoder->private_->residual_workspace_mid_side_unaligned[i][0] = encoder->private_->residual_workspace_mid_side[i][0] = 0;
		encoder->private_->residual_workspace_mid_side_unaligned[i][1] = encoder->private_->residual_workspace_mid_side[i][1] = 0;
		encoder->private_->best_subframe_mid_side[i] = 0;
	}
	encoder->private_->abs_residual_partition_sums_unaligned = encoder->private_->abs_residual_partition_sums = 0;
	encoder->private_->raw_bits_per_partition_unaligned = encoder->private_->raw_bits_per_partition = 0;
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	encoder->private_->loose_mid_side_stereo_frames = (uint32_t)((double)encoder->protected_->sample_rate * 0.4 / (double)encoder->protected_->blocksize + 0.5);
#else
	/* 26214 is the approximate fixed-point equivalent to 0.4 (0.4 * 2^16) */
	/* sample rate can be up to 655350 Hz, and thus use 20 bits, so we do the multiply&divide by hand */
	FLAC__ASSERT(FLAC__MAX_SAMPLE_RATE <= 655350);
	FLAC__ASSERT(FLAC__MAX_BLOCK_SIZE <= 65535);
	FLAC__ASSERT(encoder->protected_->sample_rate <= 655350);
	FLAC__ASSERT(encoder->protected_->blocksize <= 65535);
	encoder->private_->loose_mid_side_stereo_frames = (uint32_t)FLAC__fixedpoint_trunc((((FLAC__uint64)(encoder->protected_->sample_rate) * (FLAC__uint64)(26214)) << 16) / (encoder->protected_->blocksize<<16) + FLAC__FP_ONE_HALF);
#endif
	if(encoder->private_->loose_mid_side_stereo_frames == 0)
		encoder->private_->loose_mid_side_stereo_frames = 1;
	encoder->private_->loose_mid_side_stereo_frame_count = 0;
	encoder->private_->current_sample_number = 0;
	encoder->private_->current_frame_number = 0;

	/*
	 * get the CPU info and set the function pointers
	 */
	FLAC__cpu_info(&encoder->private_->cpuinfo);
	/* first default to the non-asm routines */
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation;
#endif
	encoder->private_->local_precompute_partition_info_sums = precompute_partition_info_sums_;
	encoder->private_->local_fixed_compute_best_predictor = FLAC__fixed_compute_best_predictor;
	encoder->private_->local_fixed_compute_best_predictor_wide = FLAC__fixed_compute_best_predictor_wide;
#ifndef FLAC__INTEGER_ONLY_LIBRARY
	encoder->private_->local_lpc_compute_residual_from_qlp_coefficients = FLAC__lpc_compute_residual_from_qlp_coefficients;
	encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_64bit = FLAC__lpc_compute_residual_from_qlp_coefficients_wide;
	encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit = FLAC__lpc_compute_residual_from_qlp_coefficients;
#endif
	/* now override with asm where appropriate */
#ifndef FLAC__INTEGER_ONLY_LIBRARY
# ifndef FLAC__NO_ASM
#if defined(FLAC__CPU_PPC64) && defined(FLAC__USE_VSX)
#ifdef FLAC__HAS_TARGET_POWER8
#ifdef FLAC__HAS_TARGET_POWER9
	if (encoder->private_->cpuinfo.ppc.arch_3_00) {
		if(encoder->protected_->max_lpc_order < 4)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power9_vsx_lag_4;
		else if(encoder->protected_->max_lpc_order < 8)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power9_vsx_lag_8;
		else if(encoder->protected_->max_lpc_order < 12)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power9_vsx_lag_12;
		else if(encoder->protected_->max_lpc_order < 16)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power9_vsx_lag_16;
		else
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation;
	} else
#endif
	if (encoder->private_->cpuinfo.ppc.arch_2_07) {
		if(encoder->protected_->max_lpc_order < 4)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power8_vsx_lag_4;
		else if(encoder->protected_->max_lpc_order < 8)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power8_vsx_lag_8;
		else if(encoder->protected_->max_lpc_order < 12)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power8_vsx_lag_12;
		else if(encoder->protected_->max_lpc_order < 16)
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_power8_vsx_lag_16;
		else
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation;
	}
#endif
#endif
	if(encoder->private_->cpuinfo.use_asm) {
#  ifdef FLAC__CPU_IA32
		FLAC__ASSERT(encoder->private_->cpuinfo.type == FLAC__CPUINFO_TYPE_IA32);
#   ifdef FLAC__HAS_NASM
		if (encoder->private_->cpuinfo.x86.sse) {
			if(encoder->protected_->max_lpc_order < 4)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_asm_ia32_sse_lag_4_old;
			else if(encoder->protected_->max_lpc_order < 8)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_asm_ia32_sse_lag_8_old;
			else if(encoder->protected_->max_lpc_order < 12)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_asm_ia32_sse_lag_12_old;
			else if(encoder->protected_->max_lpc_order < 16)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_asm_ia32_sse_lag_16_old;
			else
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_asm_ia32;
		}
		else
			encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_asm_ia32;

		encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_64bit = FLAC__lpc_compute_residual_from_qlp_coefficients_wide_asm_ia32; /* OPT_IA32: was really necessary for GCC < 4.9 */
		if (encoder->private_->cpuinfo.x86.mmx) {
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients = FLAC__lpc_compute_residual_from_qlp_coefficients_asm_ia32;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit = FLAC__lpc_compute_residual_from_qlp_coefficients_asm_ia32_mmx;
		}
		else {
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients = FLAC__lpc_compute_residual_from_qlp_coefficients_asm_ia32;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit = FLAC__lpc_compute_residual_from_qlp_coefficients_asm_ia32;
		}

		if (encoder->private_->cpuinfo.x86.mmx && encoder->private_->cpuinfo.x86.cmov)
			encoder->private_->local_fixed_compute_best_predictor = FLAC__fixed_compute_best_predictor_asm_ia32_mmx_cmov;
#   endif /* FLAC__HAS_NASM */
#   if FLAC__HAS_X86INTRIN
#    if defined FLAC__SSE_SUPPORTED
		if (encoder->private_->cpuinfo.x86.sse) {
			if (encoder->private_->cpuinfo.x86.sse42 || !encoder->private_->cpuinfo.x86.intel) { /* use new autocorrelation functions */
				if(encoder->protected_->max_lpc_order < 4)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_4_new;
				else if(encoder->protected_->max_lpc_order < 8)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_8_new;
				else if(encoder->protected_->max_lpc_order < 12)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_12_new;
				else if(encoder->protected_->max_lpc_order < 16)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_16_new;
				else
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation;
			}
			else { /* use old autocorrelation functions */
				if(encoder->protected_->max_lpc_order < 4)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_4_old;
				else if(encoder->protected_->max_lpc_order < 8)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_8_old;
				else if(encoder->protected_->max_lpc_order < 12)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_12_old;
				else if(encoder->protected_->max_lpc_order < 16)
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_16_old;
				else
					encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation;
			}
		}
#    endif

#    ifdef FLAC__SSE2_SUPPORTED
		if (encoder->private_->cpuinfo.x86.sse2) {
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients       = FLAC__lpc_compute_residual_from_qlp_coefficients_intrin_sse2;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit = FLAC__lpc_compute_residual_from_qlp_coefficients_16_intrin_sse2;
		}
#    endif
#    ifdef FLAC__SSE4_1_SUPPORTED
		if (encoder->private_->cpuinfo.x86.sse41) {
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients       = FLAC__lpc_compute_residual_from_qlp_coefficients_intrin_sse41;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_64bit = FLAC__lpc_compute_residual_from_qlp_coefficients_wide_intrin_sse41;
		}
#    endif
#    ifdef FLAC__AVX2_SUPPORTED
		if (encoder->private_->cpuinfo.x86.avx2) {
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit = FLAC__lpc_compute_residual_from_qlp_coefficients_16_intrin_avx2;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients       = FLAC__lpc_compute_residual_from_qlp_coefficients_intrin_avx2;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_64bit = FLAC__lpc_compute_residual_from_qlp_coefficients_wide_intrin_avx2;
		}
#    endif

#    ifdef FLAC__SSE2_SUPPORTED
		if (encoder->private_->cpuinfo.x86.sse2) {
			encoder->private_->local_fixed_compute_best_predictor      = FLAC__fixed_compute_best_predictor_intrin_sse2;
			encoder->private_->local_fixed_compute_best_predictor_wide = FLAC__fixed_compute_best_predictor_wide_intrin_sse2;
		}
#    endif
#    ifdef FLAC__SSSE3_SUPPORTED
		if (encoder->private_->cpuinfo.x86.ssse3) {
			encoder->private_->local_fixed_compute_best_predictor      = FLAC__fixed_compute_best_predictor_intrin_ssse3;
			encoder->private_->local_fixed_compute_best_predictor_wide = FLAC__fixed_compute_best_predictor_wide_intrin_ssse3;
		}
#    endif
#   endif /* FLAC__HAS_X86INTRIN */
#  elif defined FLAC__CPU_X86_64
		FLAC__ASSERT(encoder->private_->cpuinfo.type == FLAC__CPUINFO_TYPE_X86_64);
#   if FLAC__HAS_X86INTRIN
#    ifdef FLAC__SSE_SUPPORTED
		if(encoder->private_->cpuinfo.x86.sse42 || !encoder->private_->cpuinfo.x86.intel) { /* use new autocorrelation functions */
			if(encoder->protected_->max_lpc_order < 4)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_4_new;
			else if(encoder->protected_->max_lpc_order < 8)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_8_new;
			else if(encoder->protected_->max_lpc_order < 12)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_12_new;
			else if(encoder->protected_->max_lpc_order < 16)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_16_new;
		}
		else {
			if(encoder->protected_->max_lpc_order < 4)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_4_old;
			else if(encoder->protected_->max_lpc_order < 8)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_8_old;
			else if(encoder->protected_->max_lpc_order < 12)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_12_old;
			else if(encoder->protected_->max_lpc_order < 16)
				encoder->private_->local_lpc_compute_autocorrelation = FLAC__lpc_compute_autocorrelation_intrin_sse_lag_16_old;
		}
#    endif

#    ifdef FLAC__SSE2_SUPPORTED
		encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit = FLAC__lpc_compute_residual_from_qlp_coefficients_16_intrin_sse2;
#    endif
#    ifdef FLAC__SSE4_1_SUPPORTED
		if(encoder->private_->cpuinfo.x86.sse41) {
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients = FLAC__lpc_compute_residual_from_qlp_coefficients_intrin_sse41;
		}
#    endif
#    ifdef FLAC__AVX2_SUPPORTED
		if(encoder->private_->cpuinfo.x86.avx2) {
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit = FLAC__lpc_compute_residual_from_qlp_coefficients_16_intrin_avx2;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients       = FLAC__lpc_compute_residual_from_qlp_coefficients_intrin_avx2;
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_64bit = FLAC__lpc_compute_residual_from_qlp_coefficients_wide_intrin_avx2;
		}
#    endif

#    ifdef FLAC__SSE2_SUPPORTED
		encoder->private_->local_fixed_compute_best_predictor      = FLAC__fixed_compute_best_predictor_intrin_sse2;
		encoder->private_->local_fixed_compute_best_predictor_wide = FLAC__fixed_compute_best_predictor_wide_intrin_sse2;
#    endif
#    ifdef FLAC__SSSE3_SUPPORTED
		if (encoder->private_->cpuinfo.x86.ssse3) {
			encoder->private_->local_fixed_compute_best_predictor      = FLAC__fixed_compute_best_predictor_intrin_ssse3;
			encoder->private_->local_fixed_compute_best_predictor_wide = FLAC__fixed_compute_best_predictor_wide_intrin_ssse3;
		}
#    endif
#   endif /* FLAC__HAS_X86INTRIN */
#  endif /* FLAC__CPU_... */
	}
# endif /* !FLAC__NO_ASM */
#endif /* !FLAC__INTEGER_ONLY_LIBRARY */
#if !defined FLAC__NO_ASM && FLAC__HAS_X86INTRIN
	if(encoder->private_->cpuinfo.use_asm) {
# if defined FLAC__CPU_IA32
#  ifdef FLAC__SSE2_SUPPORTED
		if (encoder->private_->cpuinfo.x86.sse2)
			encoder->private_->local_precompute_partition_info_sums = FLAC__precompute_partition_info_sums_intrin_sse2;
#  endif
#  ifdef FLAC__SSSE3_SUPPORTED
		if (encoder->private_->cpuinfo.x86.ssse3)
			encoder->private_->local_precompute_partition_info_sums = FLAC__precompute_partition_info_sums_intrin_ssse3;
#  endif
#  ifdef FLAC__AVX2_SUPPORTED
		if (encoder->private_->cpuinfo.x86.avx2)
			encoder->private_->local_precompute_partition_info_sums = FLAC__precompute_partition_info_sums_intrin_avx2;
#  endif
# elif defined FLAC__CPU_X86_64
#  ifdef FLAC__SSE2_SUPPORTED
		encoder->private_->local_precompute_partition_info_sums = FLAC__precompute_partition_info_sums_intrin_sse2;
#  endif
#  ifdef FLAC__SSSE3_SUPPORTED
		if(encoder->private_->cpuinfo.x86.ssse3)
			encoder->private_->local_precompute_partition_info_sums = FLAC__precompute_partition_info_sums_intrin_ssse3;
#  endif
#  ifdef FLAC__AVX2_SUPPORTED
		if(encoder->private_->cpuinfo.x86.avx2)
			encoder->private_->local_precompute_partition_info_sums = FLAC__precompute_partition_info_sums_intrin_avx2;
#  endif
# endif /* FLAC__CPU_... */
	}
#endif /* !FLAC__NO_ASM && FLAC__HAS_X86INTRIN */

	/* set state to OK; from here on, errors are fatal and we'll override the state then */
	encoder->protected_->state = FLAC__STREAM_ENCODER_OK;

#if FLAC__HAS_OGG
	encoder->private_->is_ogg = is_ogg;
	if(is_ogg && !FLAC__ogg_encoder_aspect_init(&encoder->protected_->ogg_encoder_aspect)) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_OGG_ERROR;
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}
#endif

	encoder->private_->read_callback = read_callback;
	encoder->private_->write_callback = write_callback;
	encoder->private_->seek_callback = seek_callback;
	encoder->private_->tell_callback = tell_callback;
	encoder->private_->metadata_callback = metadata_callback;
	encoder->private_->client_data = client_data;

	if(!resize_buffers_(encoder, encoder->protected_->blocksize)) {
		/* the above function sets the state for us in case of an error */
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}

	if(!FLAC__bitwriter_init(encoder->private_->frame)) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR;
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}

	/*
	 * Set up the verify stuff if necessary
	 */
	if(encoder->protected_->verify) {
		/*
		 * First, set up the fifo which will hold the
		 * original signal to compare against
		 */
		encoder->private_->verify.input_fifo.size = encoder->protected_->blocksize+OVERREAD_;
		for(i = 0; i < encoder->protected_->channels; i++) {
			if(0 == (encoder->private_->verify.input_fifo.data[i] = safe_malloc_mul_2op_p(sizeof(FLAC__int32), /*times*/encoder->private_->verify.input_fifo.size))) {
				encoder->protected_->state = FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR;
				return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
			}
		}
		encoder->private_->verify.input_fifo.tail = 0;

		/*
		 * Now set up a stream decoder for verification
		 */
		if(0 == encoder->private_->verify.decoder) {
			encoder->private_->verify.decoder = FLAC__stream_decoder_new();
			if(0 == encoder->private_->verify.decoder) {
				encoder->protected_->state = FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR;
				return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
			}
		}

		if(FLAC__stream_decoder_init_stream(encoder->private_->verify.decoder, verify_read_callback_, /*seek_callback=*/0, /*tell_callback=*/0, /*length_callback=*/0, /*eof_callback=*/0, verify_write_callback_, verify_metadata_callback_, verify_error_callback_, /*client_data=*/encoder) != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
			encoder->protected_->state = FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR;
			return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
		}
	}
	encoder->private_->verify.error_stats.absolute_sample = 0;
	encoder->private_->verify.error_stats.frame_number = 0;
	encoder->private_->verify.error_stats.channel = 0;
	encoder->private_->verify.error_stats.sample = 0;
	encoder->private_->verify.error_stats.expected = 0;
	encoder->private_->verify.error_stats.got = 0;

	/*
	 * These must be done before we write any metadata, because that
	 * calls the write_callback, which uses these values.
	 */
	encoder->private_->first_seekpoint_to_check = 0;
	encoder->private_->samples_written = 0;
	encoder->protected_->streaminfo_offset = 0;
	encoder->protected_->seektable_offset = 0;
	encoder->protected_->audio_offset = 0;

	/*
	 * write the stream header
	 */
	if(encoder->protected_->verify)
		encoder->private_->verify.state_hint = ENCODER_IN_MAGIC;
	if(!FLAC__bitwriter_write_raw_uint32(encoder->private_->frame, FLAC__STREAM_SYNC, FLAC__STREAM_SYNC_LEN)) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}
	if(!write_bitbuffer_(encoder, 0, /*is_last_block=*/false)) {
		/* the above function sets the state for us in case of an error */
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}

	/*
	 * write the STREAMINFO metadata block
	 */
	if(encoder->protected_->verify)
		encoder->private_->verify.state_hint = ENCODER_IN_METADATA;
	encoder->private_->streaminfo.type = FLAC__METADATA_TYPE_STREAMINFO;
	encoder->private_->streaminfo.is_last = false; /* we will have at a minimum a VORBIS_COMMENT afterwards */
	encoder->private_->streaminfo.length = FLAC__STREAM_METADATA_STREAMINFO_LENGTH;
	encoder->private_->streaminfo.data.stream_info.min_blocksize = encoder->protected_->blocksize; /* this encoder uses the same blocksize for the whole stream */
	encoder->private_->streaminfo.data.stream_info.max_blocksize = encoder->protected_->blocksize;
	encoder->private_->streaminfo.data.stream_info.min_framesize = 0; /* we don't know this yet; have to fill it in later */
	encoder->private_->streaminfo.data.stream_info.max_framesize = 0; /* we don't know this yet; have to fill it in later */
	encoder->private_->streaminfo.data.stream_info.sample_rate = encoder->protected_->sample_rate;
	encoder->private_->streaminfo.data.stream_info.channels = encoder->protected_->channels;
	encoder->private_->streaminfo.data.stream_info.bits_per_sample = encoder->protected_->bits_per_sample;
	encoder->private_->streaminfo.data.stream_info.total_samples = encoder->protected_->total_samples_estimate; /* we will replace this later with the real total */
	memset(encoder->private_->streaminfo.data.stream_info.md5sum, 0, 16); /* we don't know this yet; have to fill it in later */
	if(encoder->protected_->do_md5)
		FLAC__MD5Init(&encoder->private_->md5context);
	if(!FLAC__add_metadata_block(&encoder->private_->streaminfo, encoder->private_->frame)) {
		encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}
	if(!write_bitbuffer_(encoder, 0, /*is_last_block=*/false)) {
		/* the above function sets the state for us in case of an error */
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}

	/*
	 * Now that the STREAMINFO block is written, we can init this to an
	 * absurdly-high value...
	 */
	encoder->private_->streaminfo.data.stream_info.min_framesize = (1u << FLAC__STREAM_METADATA_STREAMINFO_MIN_FRAME_SIZE_LEN) - 1;
	/* ... and clear this to 0 */
	encoder->private_->streaminfo.data.stream_info.total_samples = 0;

	/*
	 * Check to see if the supplied metadata contains a VORBIS_COMMENT;
	 * if not, we will write an empty one (FLAC__add_metadata_block()
	 * automatically supplies the vendor string).
	 *
	 * WATCHOUT: the Ogg FLAC mapping requires us to write this block after
	 * the STREAMINFO.  (In the case that metadata_has_vorbis_comment is
	 * true it will have already insured that the metadata list is properly
	 * ordered.)
	 */
	if(!metadata_has_vorbis_comment) {
		FLAC__StreamMetadata vorbis_comment;
		vorbis_comment.type = FLAC__METADATA_TYPE_VORBIS_COMMENT;
		vorbis_comment.is_last = (encoder->protected_->num_metadata_blocks == 0);
		vorbis_comment.length = 4 + 4; /* MAGIC NUMBER */
		vorbis_comment.data.vorbis_comment.vendor_string.length = 0;
		vorbis_comment.data.vorbis_comment.vendor_string.entry = 0;
		vorbis_comment.data.vorbis_comment.num_comments = 0;
		vorbis_comment.data.vorbis_comment.comments = 0;
		if(!FLAC__add_metadata_block(&vorbis_comment, encoder->private_->frame)) {
			encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
			return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
		}
		if(!write_bitbuffer_(encoder, 0, /*is_last_block=*/false)) {
			/* the above function sets the state for us in case of an error */
			return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
		}
	}

	/*
	 * write the user's metadata blocks
	 */
	for(i = 0; i < encoder->protected_->num_metadata_blocks; i++) {
		encoder->protected_->metadata[i]->is_last = (i == encoder->protected_->num_metadata_blocks - 1);
		if(!FLAC__add_metadata_block(encoder->protected_->metadata[i], encoder->private_->frame)) {
			encoder->protected_->state = FLAC__STREAM_ENCODER_FRAMING_ERROR;
			return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
		}
		if(!write_bitbuffer_(encoder, 0, /*is_last_block=*/false)) {
			/* the above function sets the state for us in case of an error */
			return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
		}
	}

	/* now that all the metadata is written, we save the stream offset */
	if(encoder->private_->tell_callback && encoder->private_->tell_callback(encoder, &encoder->protected_->audio_offset, encoder->private_->client_data) == FLAC__STREAM_ENCODER_TELL_STATUS_ERROR) { /* FLAC__STREAM_ENCODER_TELL_STATUS_UNSUPPORTED just means we didn't get the offset; no error */
		encoder->protected_->state = FLAC__STREAM_ENCODER_CLIENT_ERROR;
		return FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
	}

	if(encoder->protected_->verify)
		encoder->private_->verify.state_hint = ENCODER_IN_AUDIO;

	return FLAC__STREAM_ENCODER_INIT_STATUS_OK;
}