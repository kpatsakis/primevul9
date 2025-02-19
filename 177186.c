ext4_xattr_cmp(struct ext4_xattr_header *header1,
	       struct ext4_xattr_header *header2)
{
	struct ext4_xattr_entry *entry1, *entry2;

	entry1 = ENTRY(header1+1);
	entry2 = ENTRY(header2+1);
	while (!IS_LAST_ENTRY(entry1)) {
		if (IS_LAST_ENTRY(entry2))
			return 1;
		if (entry1->e_hash != entry2->e_hash ||
		    entry1->e_name_index != entry2->e_name_index ||
		    entry1->e_name_len != entry2->e_name_len ||
		    entry1->e_value_size != entry2->e_value_size ||
		    entry1->e_value_inum != entry2->e_value_inum ||
		    memcmp(entry1->e_name, entry2->e_name, entry1->e_name_len))
			return 1;
		if (!entry1->e_value_inum &&
		    memcmp((char *)header1 + le16_to_cpu(entry1->e_value_offs),
			   (char *)header2 + le16_to_cpu(entry2->e_value_offs),
			   le32_to_cpu(entry1->e_value_size)))
			return 1;

		entry1 = EXT4_XATTR_NEXT(entry1);
		entry2 = EXT4_XATTR_NEXT(entry2);
	}
	if (!IS_LAST_ENTRY(entry2))
		return 1;
	return 0;
}