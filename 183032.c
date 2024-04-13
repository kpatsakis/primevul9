setrl(isc_ratelimiter_t *rl, unsigned int *rate, unsigned int value) {
	isc_interval_t interval;
	uint32_t s, ns;
	uint32_t pertic;
	isc_result_t result;

	if (value == 0)
		value = 1;

	if (value == 1) {
		s = 1;
		ns = 0;
		pertic = 1;
	} else if (value <= 10) {
		s = 0;
		ns = 1000000000 / value;
		pertic = 1;
	} else {
		s = 0;
		ns = (1000000000 / value) * 10;
		pertic = 10;
	}

	isc_interval_set(&interval, s, ns);

	result = isc_ratelimiter_setinterval(rl, &interval);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	isc_ratelimiter_setpertic(rl, pertic);

	*rate = value;
}