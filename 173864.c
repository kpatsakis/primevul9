static s32 lg4ff_adjust_dfp_x_axis(s32 value, u16 range)
{
	u16 max_range;
	s32 new_value;

	if (range == 900)
		return value;
	else if (range == 200)
		return value;
	else if (range < 200)
		max_range = 200;
	else
		max_range = 900;

	new_value = 8192 + mult_frac(value - 8192, max_range, range);
	if (new_value < 0)
		return 0;
	else if (new_value > 16383)
		return 16383;
	else
		return new_value;
}