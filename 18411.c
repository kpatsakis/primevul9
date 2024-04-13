void add_input_randomness(unsigned int type, unsigned int code,
				 unsigned int value)
{
	static unsigned char last_value;

	/* ignore autorepeat and the like */
	if (value == last_value)
		return;

	last_value = value;
	add_timer_randomness(&input_timer_state,
			     (type << 4) ^ code ^ (code >> 4) ^ value);
	trace_add_input_randomness(ENTROPY_BITS(&input_pool));
}