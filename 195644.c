void ext4_superblock_csum_set(struct super_block *sb,
			      struct ext4_super_block *es)
{
	if (!EXT4_HAS_RO_COMPAT_FEATURE(sb,
		EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		return;

	es->s_checksum = ext4_superblock_csum(sb, es);
}