sec_hash_to_string(char *out, int out_size, uint8 * in, int in_size)
{
	int k;
	memset(out, 0, out_size);
	for (k = 0; k < in_size; k++, out += 2)
	{
		sprintf(out, "%.2x", in[k]);
	}
}