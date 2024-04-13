void ext4_group_desc_csum_set(struct super_block *sb, __u32 block_group,
			      struct ext4_group_desc *gdp)
{
	if (!ext4_has_group_desc_csum(sb))
		return;
	gdp->bg_checksum = ext4_group_desc_csum(EXT4_SB(sb), block_group, gdp);
}