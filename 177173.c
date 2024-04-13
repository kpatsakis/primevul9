static int ext4_xattr_value_same(struct ext4_xattr_search *s,
				 struct ext4_xattr_info *i)
{
	void *value;

	/* When e_value_inum is set the value is stored externally. */
	if (s->here->e_value_inum)
		return 0;
	if (le32_to_cpu(s->here->e_value_size) != i->value_len)
		return 0;
	value = ((void *)s->base) + le16_to_cpu(s->here->e_value_offs);
	return !memcmp(value, i->value, i->value_len);
}