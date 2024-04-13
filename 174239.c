static int may_create(struct inode *dir,
		      struct dentry *dentry,
		      u16 tclass)
{
	const struct task_security_struct *tsec = selinux_cred(current_cred());
	struct inode_security_struct *dsec;
	struct superblock_security_struct *sbsec;
	u32 sid, newsid;
	struct common_audit_data ad;
	int rc;

	dsec = inode_security(dir);
	sbsec = dir->i_sb->s_security;

	sid = tsec->sid;

	ad.type = LSM_AUDIT_DATA_DENTRY;
	ad.u.dentry = dentry;

	rc = avc_has_perm(&selinux_state,
			  sid, dsec->sid, SECCLASS_DIR,
			  DIR__ADD_NAME | DIR__SEARCH,
			  &ad);
	if (rc)
		return rc;

	rc = selinux_determine_inode_label(tsec, dir, &dentry->d_name, tclass,
					   &newsid);
	if (rc)
		return rc;

	rc = avc_has_perm(&selinux_state,
			  sid, newsid, tclass, FILE__CREATE, &ad);
	if (rc)
		return rc;

	return avc_has_perm(&selinux_state,
			    newsid, sbsec->sid,
			    SECCLASS_FILESYSTEM,
			    FILESYSTEM__ASSOCIATE, &ad);
}