static __le16 ext4_group_desc_csum(struct ext4_sb_info *sbi, __u32 block_group,
				   struct ext4_group_desc *gdp)
{
	int offset;
	__u16 crc = 0;
	__le32 le_group = cpu_to_le32(block_group);

	if ((sbi->s_es->s_feature_ro_compat &
	     cpu_to_le32(EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))) {
		/* Use new metadata_csum algorithm */
		__u16 old_csum;
		__u32 csum32;

		old_csum = gdp->bg_checksum;
		gdp->bg_checksum = 0;
		csum32 = ext4_chksum(sbi, sbi->s_csum_seed, (__u8 *)&le_group,
				     sizeof(le_group));
		csum32 = ext4_chksum(sbi, csum32, (__u8 *)gdp,
				     sbi->s_desc_size);
		gdp->bg_checksum = old_csum;

		crc = csum32 & 0xFFFF;
		goto out;
	}

	/* old crc16 code */
	offset = offsetof(struct ext4_group_desc, bg_checksum);

	crc = crc16(~0, sbi->s_es->s_uuid, sizeof(sbi->s_es->s_uuid));
	crc = crc16(crc, (__u8 *)&le_group, sizeof(le_group));
	crc = crc16(crc, (__u8 *)gdp, offset);
	offset += sizeof(gdp->bg_checksum); /* skip checksum */
	/* for checksum of struct ext4_group_desc do the rest...*/
	if ((sbi->s_es->s_feature_incompat &
	     cpu_to_le32(EXT4_FEATURE_INCOMPAT_64BIT)) &&
	    offset < le16_to_cpu(sbi->s_es->s_desc_size))
		crc = crc16(crc, (__u8 *)gdp + offset,
			    le16_to_cpu(sbi->s_es->s_desc_size) -
				offset);

out:
	return cpu_to_le16(crc);
}