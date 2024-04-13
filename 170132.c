static unsigned long hash_key_type_and_desc(const struct keyring_index_key *index_key)
{
	const unsigned level_shift = ASSOC_ARRAY_LEVEL_STEP;
	const unsigned long fan_mask = ASSOC_ARRAY_FAN_MASK;
	const char *description = index_key->description;
	unsigned long hash, type;
	u32 piece;
	u64 acc;
	int n, desc_len = index_key->desc_len;

	type = (unsigned long)index_key->type;

	acc = mult_64x32_and_fold(type, desc_len + 13);
	acc = mult_64x32_and_fold(acc, 9207);
	for (;;) {
		n = desc_len;
		if (n <= 0)
			break;
		if (n > 4)
			n = 4;
		piece = 0;
		memcpy(&piece, description, n);
		description += n;
		desc_len -= n;
		acc = mult_64x32_and_fold(acc, piece);
		acc = mult_64x32_and_fold(acc, 9207);
	}

	/* Fold the hash down to 32 bits if need be. */
	hash = acc;
	if (ASSOC_ARRAY_KEY_CHUNK_SIZE == 32)
		hash ^= acc >> 32;

	/* Squidge all the keyrings into a separate part of the tree to
	 * ordinary keys by making sure the lowest level segment in the hash is
	 * zero for keyrings and non-zero otherwise.
	 */
	if (index_key->type != &key_type_keyring && (hash & fan_mask) == 0)
		return hash | (hash >> (ASSOC_ARRAY_KEY_CHUNK_SIZE - level_shift)) | 1;
	if (index_key->type == &key_type_keyring && (hash & fan_mask) != 0)
		return (hash + (hash << level_shift)) & ~fan_mask;
	return hash;
}