static int selinux_inode_follow_link(struct dentry *dentry, struct inode *inode,
				     bool rcu)
{
	const struct cred *cred = current_cred();
	struct common_audit_data ad;
	struct inode_security_struct *isec;
	u32 sid;

	validate_creds(cred);

	ad.type = LSM_AUDIT_DATA_DENTRY;
	ad.u.dentry = dentry;
	sid = cred_sid(cred);
	isec = inode_security_rcu(inode, rcu);
	if (IS_ERR(isec))
		return PTR_ERR(isec);

	return avc_has_perm_flags(&selinux_state,
				  sid, isec->sid, isec->sclass, FILE__READ, &ad,
				  rcu ? MAY_NOT_BLOCK : 0);
}