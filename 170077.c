find_bits (int n, int *lowest, int *n_set)
{
	int i;

	*n_set = 0;

	for (i = 31; i >= 0; i--)
		if (n & (1 << i)) {
			*lowest = i;
			(*n_set)++;
		}
}