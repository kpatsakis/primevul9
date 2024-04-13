static int superblock_has_perm(const struct cred *cred,
			       struct super_block *sb,
			       u32 perms,
			       struct common_audit_data *ad)
{
	struct superblock_security_struct *sbsec;
	u32 sid = cred_sid(cred);

	sbsec = sb->s_security;
	return avc_has_perm(&selinux_state,
			    sid, sbsec->sid, SECCLASS_FILESYSTEM, perms, ad);
}