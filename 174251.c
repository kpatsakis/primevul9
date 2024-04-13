static int selinux_bprm_set_creds(struct linux_binprm *bprm)
{
	const struct task_security_struct *old_tsec;
	struct task_security_struct *new_tsec;
	struct inode_security_struct *isec;
	struct common_audit_data ad;
	struct inode *inode = file_inode(bprm->file);
	int rc;

	/* SELinux context only depends on initial program or script and not
	 * the script interpreter */
	if (bprm->called_set_creds)
		return 0;

	old_tsec = selinux_cred(current_cred());
	new_tsec = selinux_cred(bprm->cred);
	isec = inode_security(inode);

	/* Default to the current task SID. */
	new_tsec->sid = old_tsec->sid;
	new_tsec->osid = old_tsec->sid;

	/* Reset fs, key, and sock SIDs on execve. */
	new_tsec->create_sid = 0;
	new_tsec->keycreate_sid = 0;
	new_tsec->sockcreate_sid = 0;

	if (old_tsec->exec_sid) {
		new_tsec->sid = old_tsec->exec_sid;
		/* Reset exec SID on execve. */
		new_tsec->exec_sid = 0;

		/* Fail on NNP or nosuid if not an allowed transition. */
		rc = check_nnp_nosuid(bprm, old_tsec, new_tsec);
		if (rc)
			return rc;
	} else {
		/* Check for a default transition on this program. */
		rc = security_transition_sid(&selinux_state, old_tsec->sid,
					     isec->sid, SECCLASS_PROCESS, NULL,
					     &new_tsec->sid);
		if (rc)
			return rc;

		/*
		 * Fallback to old SID on NNP or nosuid if not an allowed
		 * transition.
		 */
		rc = check_nnp_nosuid(bprm, old_tsec, new_tsec);
		if (rc)
			new_tsec->sid = old_tsec->sid;
	}

	ad.type = LSM_AUDIT_DATA_FILE;
	ad.u.file = bprm->file;

	if (new_tsec->sid == old_tsec->sid) {
		rc = avc_has_perm(&selinux_state,
				  old_tsec->sid, isec->sid,
				  SECCLASS_FILE, FILE__EXECUTE_NO_TRANS, &ad);
		if (rc)
			return rc;
	} else {
		/* Check permissions for the transition. */
		rc = avc_has_perm(&selinux_state,
				  old_tsec->sid, new_tsec->sid,
				  SECCLASS_PROCESS, PROCESS__TRANSITION, &ad);
		if (rc)
			return rc;

		rc = avc_has_perm(&selinux_state,
				  new_tsec->sid, isec->sid,
				  SECCLASS_FILE, FILE__ENTRYPOINT, &ad);
		if (rc)
			return rc;

		/* Check for shared state */
		if (bprm->unsafe & LSM_UNSAFE_SHARE) {
			rc = avc_has_perm(&selinux_state,
					  old_tsec->sid, new_tsec->sid,
					  SECCLASS_PROCESS, PROCESS__SHARE,
					  NULL);
			if (rc)
				return -EPERM;
		}

		/* Make sure that anyone attempting to ptrace over a task that
		 * changes its SID has the appropriate permit */
		if (bprm->unsafe & LSM_UNSAFE_PTRACE) {
			u32 ptsid = ptrace_parent_sid();
			if (ptsid != 0) {
				rc = avc_has_perm(&selinux_state,
						  ptsid, new_tsec->sid,
						  SECCLASS_PROCESS,
						  PROCESS__PTRACE, NULL);
				if (rc)
					return -EPERM;
			}
		}

		/* Clear any possibly unsafe personality bits on exec: */
		bprm->per_clear |= PER_CLEAR_ON_SETID;

		/* Enable secure mode for SIDs transitions unless
		   the noatsecure permission is granted between
		   the two SIDs, i.e. ahp returns 0. */
		rc = avc_has_perm(&selinux_state,
				  old_tsec->sid, new_tsec->sid,
				  SECCLASS_PROCESS, PROCESS__NOATSECURE,
				  NULL);
		bprm->secureexec |= !!rc;
	}

	return 0;
}