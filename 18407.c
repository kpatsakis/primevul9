static int credit_entropy_bits_safe(struct entropy_store *r, int nbits)
{
	const int nbits_max = r->poolinfo->poolwords * 32;

	if (nbits < 0)
		return -EINVAL;

	/* Cap the value to avoid overflows */
	nbits = min(nbits,  nbits_max);

	credit_entropy_bits(r, nbits);
	return 0;
}