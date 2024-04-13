static int get_compatible_type(struct ff_device *ff, int effect_type)
{

	if (test_bit(effect_type, ff->ffbit))
		return effect_type;

	if (effect_type == FF_PERIODIC && test_bit(FF_RUMBLE, ff->ffbit))
		return FF_RUMBLE;

	pr_err("invalid type in get_compatible_type()\n");

	return 0;
}