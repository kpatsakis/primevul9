next_keyevent(dst_key_t *key, isc_stdtime_t *timep) {
	isc_result_t result;
	isc_stdtime_t now, then = 0, event;
	int i;

	now = *timep;

	for (i = 0; i <= DST_MAX_TIMES; i++) {
		result = dst_key_gettime(key, i, &event);
		if (result == ISC_R_SUCCESS && event > now &&
		    (then == 0 || event < then))
			then = event;
	}

	if (then != 0) {
		*timep = then;
		return (ISC_R_SUCCESS);
	}

	return (ISC_R_NOTFOUND);
}