static inline unsigned long hash_name(const char *name, unsigned int *hashp)
{
	unsigned long a, b, adata, bdata, mask, hash, len;
	const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;

	hash = a = 0;
	len = -sizeof(unsigned long);
	do {
		hash = (hash + a) * 9;
		len += sizeof(unsigned long);
		a = load_unaligned_zeropad(name+len);
		b = a ^ REPEAT_BYTE('/');
	} while (!(has_zero(a, &adata, &constants) | has_zero(b, &bdata, &constants)));

	adata = prep_zero_mask(a, adata, &constants);
	bdata = prep_zero_mask(b, bdata, &constants);

	mask = create_zero_mask(adata | bdata);

	hash += a & zero_bytemask(mask);
	*hashp = fold_hash(hash);

	return len + find_zero(mask);
}