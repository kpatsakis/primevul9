static int ext4_enable_quotas(struct super_block *sb)
{
	int type, err = 0;
	unsigned long qf_inums[EXT4_MAXQUOTAS] = {
		le32_to_cpu(EXT4_SB(sb)->s_es->s_usr_quota_inum),
		le32_to_cpu(EXT4_SB(sb)->s_es->s_grp_quota_inum),
		le32_to_cpu(EXT4_SB(sb)->s_es->s_prj_quota_inum)
	};
	bool quota_mopt[EXT4_MAXQUOTAS] = {
		test_opt(sb, USRQUOTA),
		test_opt(sb, GRPQUOTA),
		test_opt(sb, PRJQUOTA),
	};

	sb_dqopt(sb)->flags |= DQUOT_QUOTA_SYS_FILE | DQUOT_NOLIST_DIRTY;
	for (type = 0; type < EXT4_MAXQUOTAS; type++) {
		if (qf_inums[type]) {
			err = ext4_quota_enable(sb, type, QFMT_VFS_V1,
				DQUOT_USAGE_ENABLED |
				(quota_mopt[type] ? DQUOT_LIMITS_ENABLED : 0));
			if (err) {
				ext4_warning(sb,
					"Failed to enable quota tracking "
					"(type=%d, err=%d). Please run "
					"e2fsck to fix.", type, err);
				for (type--; type >= 0; type--)
					dquot_quota_off(sb, type);

				return err;
			}
		}
	}
	return 0;
}