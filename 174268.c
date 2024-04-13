static int selinux_sb_remount(struct super_block *sb, void *mnt_opts)
{
	struct selinux_mnt_opts *opts = mnt_opts;
	struct superblock_security_struct *sbsec = sb->s_security;
	u32 sid;
	int rc;

	if (!(sbsec->flags & SE_SBINITIALIZED))
		return 0;

	if (!opts)
		return 0;

	if (opts->fscontext) {
		rc = parse_sid(sb, opts->fscontext, &sid);
		if (rc)
			return rc;
		if (bad_option(sbsec, FSCONTEXT_MNT, sbsec->sid, sid))
			goto out_bad_option;
	}
	if (opts->context) {
		rc = parse_sid(sb, opts->context, &sid);
		if (rc)
			return rc;
		if (bad_option(sbsec, CONTEXT_MNT, sbsec->mntpoint_sid, sid))
			goto out_bad_option;
	}
	if (opts->rootcontext) {
		struct inode_security_struct *root_isec;
		root_isec = backing_inode_security(sb->s_root);
		rc = parse_sid(sb, opts->rootcontext, &sid);
		if (rc)
			return rc;
		if (bad_option(sbsec, ROOTCONTEXT_MNT, root_isec->sid, sid))
			goto out_bad_option;
	}
	if (opts->defcontext) {
		rc = parse_sid(sb, opts->defcontext, &sid);
		if (rc)
			return rc;
		if (bad_option(sbsec, DEFCONTEXT_MNT, sbsec->def_sid, sid))
			goto out_bad_option;
	}
	return 0;

out_bad_option:
	pr_warn("SELinux: unable to change security options "
	       "during remount (dev %s, type=%s)\n", sb->s_id,
	       sb->s_type->name);
	return -EINVAL;
}