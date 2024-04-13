static void get_one_mondo_bits(const u64 *p, unsigned int *mask,
			       unsigned long def, unsigned long max)
{
	u64 val;

	if (!p)
		goto use_default;
	val = *p;

	if (!val || val >= 64)
		goto use_default;

	if (val > max)
		val = max;

	*mask = ((1U << val) * 64U) - 1U;
	return;

use_default:
	*mask = ((1U << def) * 64U) - 1U;
}