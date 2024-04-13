static inline double php_round_helper(double value, int mode) {
	double tmp_value;

	if (value >= 0.0) {
		tmp_value = floor(value + 0.5);
		if ((mode == PHP_ROUND_HALF_DOWN && value == (-0.5 + tmp_value)) ||
			(mode == PHP_ROUND_HALF_EVEN && value == (0.5 + 2 * floor(tmp_value/2.0))) ||
			(mode == PHP_ROUND_HALF_ODD  && value == (0.5 + 2 * floor(tmp_value/2.0) - 1.0)))
		{
			tmp_value = tmp_value - 1.0;
		}
	} else {
		tmp_value = ceil(value - 0.5);
		if ((mode == PHP_ROUND_HALF_DOWN && value == (0.5 + tmp_value)) ||
			(mode == PHP_ROUND_HALF_EVEN && value == (-0.5 + 2 * ceil(tmp_value/2.0))) ||
			(mode == PHP_ROUND_HALF_ODD  && value == (-0.5 + 2 * ceil(tmp_value/2.0) + 1.0)))
		{
			tmp_value = tmp_value + 1.0;
		}
	}

	return tmp_value;
}