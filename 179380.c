multiply_alpha (int alpha, int color)
{
	int temp = (alpha * color) + 0x80;

	return ((temp + (temp >> 8)) >> 8);
}