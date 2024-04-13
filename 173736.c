static inline int tmff_scale_s8(int in, int minimum, int maximum)
{
	int ret;

	ret = (((in + 0x80) * (maximum - minimum)) / 0xff) + minimum;
	if (ret < minimum)
		return minimum;
	if (ret > maximum)
		return maximum;
	return ret;
}