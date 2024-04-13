static int ext4_set_bitmap_checksums(struct super_block *sb,
				     ext4_group_t group,
				     struct ext4_group_desc *gdp,
				     struct ext4_new_group_data *group_data)
{
	struct buffer_head *bh;

	if (!EXT4_HAS_RO_COMPAT_FEATURE(sb,
					EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		return 0;

	bh = ext4_get_bitmap(sb, group_data->inode_bitmap);
	if (!bh)
		return -EIO;
	ext4_inode_bitmap_csum_set(sb, group, gdp, bh,
				   EXT4_INODES_PER_GROUP(sb) / 8);
	brelse(bh);

	bh = ext4_get_bitmap(sb, group_data->block_bitmap);
	if (!bh)
		return -EIO;
	ext4_block_bitmap_csum_set(sb, group, gdp, bh,
				   EXT4_BLOCKS_PER_GROUP(sb) / 8);
	brelse(bh);

	return 0;
}