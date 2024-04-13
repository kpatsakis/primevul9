static int ext4_quota_enable(struct super_block *sb, int type, int format_id,
			     unsigned int flags)
{
	int err;
	struct inode *qf_inode;
	unsigned long qf_inums[EXT4_MAXQUOTAS] = {
		le32_to_cpu(EXT4_SB(sb)->s_es->s_usr_quota_inum),
		le32_to_cpu(EXT4_SB(sb)->s_es->s_grp_quota_inum),
		le32_to_cpu(EXT4_SB(sb)->s_es->s_prj_quota_inum)
	};

	BUG_ON(!ext4_has_feature_quota(sb));

	if (!qf_inums[type])
		return -EPERM;

	qf_inode = ext4_iget(sb, qf_inums[type], EXT4_IGET_SPECIAL);
	if (IS_ERR(qf_inode)) {
		ext4_error(sb, "Bad quota inode # %lu", qf_inums[type]);
		return PTR_ERR(qf_inode);
	}

	/* Don't account quota for quota files to avoid recursion */
	qf_inode->i_flags |= S_NOQUOTA;
	lockdep_set_quota_inode(qf_inode, I_DATA_SEM_QUOTA);
	err = dquot_enable(qf_inode, type, format_id, flags);
	if (err)
		lockdep_set_quota_inode(qf_inode, I_DATA_SEM_NORMAL);
	iput(qf_inode);

	return err;
}