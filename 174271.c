static int selinux_sb_show_options(struct seq_file *m, struct super_block *sb)
{
	struct superblock_security_struct *sbsec = sb->s_security;
	int rc;

	if (!(sbsec->flags & SE_SBINITIALIZED))
		return 0;

	if (!selinux_initialized(&selinux_state))
		return 0;

	if (sbsec->flags & FSCONTEXT_MNT) {
		seq_putc(m, ',');
		seq_puts(m, FSCONTEXT_STR);
		rc = show_sid(m, sbsec->sid);
		if (rc)
			return rc;
	}
	if (sbsec->flags & CONTEXT_MNT) {
		seq_putc(m, ',');
		seq_puts(m, CONTEXT_STR);
		rc = show_sid(m, sbsec->mntpoint_sid);
		if (rc)
			return rc;
	}
	if (sbsec->flags & DEFCONTEXT_MNT) {
		seq_putc(m, ',');
		seq_puts(m, DEFCONTEXT_STR);
		rc = show_sid(m, sbsec->def_sid);
		if (rc)
			return rc;
	}
	if (sbsec->flags & ROOTCONTEXT_MNT) {
		struct dentry *root = sbsec->sb->s_root;
		struct inode_security_struct *isec = backing_inode_security(root);
		seq_putc(m, ',');
		seq_puts(m, ROOTCONTEXT_STR);
		rc = show_sid(m, isec->sid);
		if (rc)
			return rc;
	}
	if (sbsec->flags & SBLABEL_MNT) {
		seq_putc(m, ',');
		seq_puts(m, SECLABEL_STR);
	}
	return 0;
}