FLAC_API FLAC__bool FLAC__stream_encoder_set_apodization(FLAC__StreamEncoder *encoder, const char *specification)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	FLAC__ASSERT(0 != specification);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
#ifdef FLAC__INTEGER_ONLY_LIBRARY
	(void)specification; /* silently ignore since we haven't integerized; will always use a rectangular window */
#else
	encoder->protected_->num_apodizations = 0;
	while(1) {
		const char *s = strchr(specification, ';');
		const size_t n = s? (size_t)(s - specification) : strlen(specification);
		if     (n==8  && 0 == strncmp("bartlett"     , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_BARTLETT;
		else if(n==13 && 0 == strncmp("bartlett_hann", specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_BARTLETT_HANN;
		else if(n==8  && 0 == strncmp("blackman"     , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_BLACKMAN;
		else if(n==26 && 0 == strncmp("blackman_harris_4term_92db", specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_BLACKMAN_HARRIS_4TERM_92DB_SIDELOBE;
		else if(n==6  && 0 == strncmp("connes"       , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_CONNES;
		else if(n==7  && 0 == strncmp("flattop"      , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_FLATTOP;
		else if(n>7   && 0 == strncmp("gauss("       , specification, 6)) {
			FLAC__real stddev = (FLAC__real)strtod(specification+6, 0);
			if (stddev > 0.0 && stddev <= 0.5) {
				encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.gauss.stddev = stddev;
				encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_GAUSS;
			}
		}
		else if(n==7  && 0 == strncmp("hamming"      , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_HAMMING;
		else if(n==4  && 0 == strncmp("hann"         , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_HANN;
		else if(n==13 && 0 == strncmp("kaiser_bessel", specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_KAISER_BESSEL;
		else if(n==7  && 0 == strncmp("nuttall"      , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_NUTTALL;
		else if(n==9  && 0 == strncmp("rectangle"    , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_RECTANGLE;
		else if(n==8  && 0 == strncmp("triangle"     , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_TRIANGLE;
		else if(n>7   && 0 == strncmp("tukey("       , specification, 6)) {
			FLAC__real p = (FLAC__real)strtod(specification+6, 0);
			if (p >= 0.0 && p <= 1.0) {
				encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.tukey.p = p;
				encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_TUKEY;
			}
		}
		else if(n>15   && 0 == strncmp("partial_tukey("       , specification, 14)) {
			FLAC__int32 tukey_parts = (FLAC__int32)strtod(specification+14, 0);
			const char *si_1 = strchr(specification, '/');
			FLAC__real overlap = si_1?flac_min((FLAC__real)strtod(si_1+1, 0),0.99f):0.1f;
			FLAC__real overlap_units = 1.0f/(1.0f - overlap) - 1.0f;
			const char *si_2 = strchr((si_1?(si_1+1):specification), '/');
			FLAC__real tukey_p = si_2?(FLAC__real)strtod(si_2+1, 0):0.2f;

			if (tukey_parts <= 1) {
				encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.tukey.p = tukey_p;
				encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_TUKEY;
			}else if (encoder->protected_->num_apodizations + tukey_parts < 32){
				FLAC__int32 m;
				for(m = 0; m < tukey_parts; m++){
					encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.multiple_tukey.p = tukey_p;
					encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.multiple_tukey.start = m/(tukey_parts+overlap_units);
					encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.multiple_tukey.end = (m+1+overlap_units)/(tukey_parts+overlap_units);
					encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_PARTIAL_TUKEY;
				}
			}
		}
		else if(n>16   && 0 == strncmp("punchout_tukey("       , specification, 15)) {
			FLAC__int32 tukey_parts = (FLAC__int32)strtod(specification+15, 0);
			const char *si_1 = strchr(specification, '/');
			FLAC__real overlap = si_1?flac_min((FLAC__real)strtod(si_1+1, 0),0.99f):0.2f;
			FLAC__real overlap_units = 1.0f/(1.0f - overlap) - 1.0f;
			const char *si_2 = strchr((si_1?(si_1+1):specification), '/');
			FLAC__real tukey_p = si_2?(FLAC__real)strtod(si_2+1, 0):0.2f;

			if (tukey_parts <= 1) {
				encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.tukey.p = tukey_p;
				encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_TUKEY;
			}else if (encoder->protected_->num_apodizations + tukey_parts < 32){
				FLAC__int32 m;
				for(m = 0; m < tukey_parts; m++){
					encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.multiple_tukey.p = tukey_p;
					encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.multiple_tukey.start = m/(tukey_parts+overlap_units);
					encoder->protected_->apodizations[encoder->protected_->num_apodizations].parameters.multiple_tukey.end = (m+1+overlap_units)/(tukey_parts+overlap_units);
					encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_PUNCHOUT_TUKEY;
				}
			}
		}
		else if(n==5  && 0 == strncmp("welch"        , specification, n))
			encoder->protected_->apodizations[encoder->protected_->num_apodizations++].type = FLAC__APODIZATION_WELCH;
		if (encoder->protected_->num_apodizations == 32)
			break;
		if (s)
			specification = s+1;
		else
			break;
	}
	if(encoder->protected_->num_apodizations == 0) {
		encoder->protected_->num_apodizations = 1;
		encoder->protected_->apodizations[0].type = FLAC__APODIZATION_TUKEY;
		encoder->protected_->apodizations[0].parameters.tukey.p = 0.5;
	}
#endif
	return true;
}