int jpc_floorlog2(int x)
{
	int y;

	/* The argument must be positive. */
	assert(x > 0);

	y = 0;
	while (x > 1) {
		x >>= 1;
		++y;
	}
	return y;
}