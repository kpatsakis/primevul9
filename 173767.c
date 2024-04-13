static inline int tmff_scale_u16(unsigned int in, int minimum, int maximum)
{
	int ret;

	ret = (in * (maximum - minimum) / 0xffff) + minimum;
	if (ret < minimum)
		return minimum;
	if (ret > maximum)
		return maximum;
	return ret;
}