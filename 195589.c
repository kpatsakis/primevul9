static int ext4_verify_csum_type(struct super_block *sb,
				 struct ext4_super_block *es)
{
	if (!EXT4_HAS_RO_COMPAT_FEATURE(sb,
					EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		return 1;

	return es->s_checksum_type == EXT4_CRC32C_CHKSUM;
}