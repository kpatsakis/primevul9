int ext4_group_desc_csum_verify(struct super_block *sb, __u32 block_group,
				struct ext4_group_desc *gdp)
{
	if (ext4_has_group_desc_csum(sb) &&
	    (gdp->bg_checksum != ext4_group_desc_csum(EXT4_SB(sb),
						      block_group, gdp)))
		return 0;

	return 1;
}