static int ext4_sb_read_encoding(const struct ext4_super_block *es,
				 const struct ext4_sb_encodings **encoding,
				 __u16 *flags)
{
	__u16 magic = le16_to_cpu(es->s_encoding);
	int i;

	for (i = 0; i < ARRAY_SIZE(ext4_sb_encoding_map); i++)
		if (magic == ext4_sb_encoding_map[i].magic)
			break;

	if (i >= ARRAY_SIZE(ext4_sb_encoding_map))
		return -EINVAL;

	*encoding = &ext4_sb_encoding_map[i];
	*flags = le16_to_cpu(es->s_encoding_flags);

	return 0;
}