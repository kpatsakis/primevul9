static noinline int audit_inode_permission(struct inode *inode,
					   u32 perms, u32 audited, u32 denied,
					   int result)
{
	struct common_audit_data ad;
	struct inode_security_struct *isec = selinux_inode(inode);
	int rc;

	ad.type = LSM_AUDIT_DATA_INODE;
	ad.u.inode = inode;

	rc = slow_avc_audit(&selinux_state,
			    current_sid(), isec->sid, isec->sclass, perms,
			    audited, denied, result, &ad);
	if (rc)
		return rc;
	return 0;
}