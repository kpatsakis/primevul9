static int selinux_inode_permission(struct inode *inode, int mask)
{
	const struct cred *cred = current_cred();
	u32 perms;
	bool from_access;
	bool no_block = mask & MAY_NOT_BLOCK;
	struct inode_security_struct *isec;
	u32 sid;
	struct av_decision avd;
	int rc, rc2;
	u32 audited, denied;

	from_access = mask & MAY_ACCESS;
	mask &= (MAY_READ|MAY_WRITE|MAY_EXEC|MAY_APPEND);

	/* No permission to check.  Existence test. */
	if (!mask)
		return 0;

	validate_creds(cred);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	perms = file_mask_to_av(inode->i_mode, mask);

	sid = cred_sid(cred);
	isec = inode_security_rcu(inode, no_block);
	if (IS_ERR(isec))
		return PTR_ERR(isec);

	rc = avc_has_perm_noaudit(&selinux_state,
				  sid, isec->sid, isec->sclass, perms,
				  no_block ? AVC_NONBLOCKING : 0,
				  &avd);
	audited = avc_audit_required(perms, &avd, rc,
				     from_access ? FILE__AUDIT_ACCESS : 0,
				     &denied);
	if (likely(!audited))
		return rc;

	/* fall back to ref-walk if we have to generate audit */
	if (no_block)
		return -ECHILD;

	rc2 = audit_inode_permission(inode, perms, audited, denied, rc);
	if (rc2)
		return rc2;
	return rc;
}