static int selinux_binder_transaction(struct task_struct *from,
				      struct task_struct *to)
{
	u32 mysid = current_sid();
	u32 fromsid = task_sid(from);
	u32 tosid = task_sid(to);
	int rc;

	if (mysid != fromsid) {
		rc = avc_has_perm(&selinux_state,
				  mysid, fromsid, SECCLASS_BINDER,
				  BINDER__IMPERSONATE, NULL);
		if (rc)
			return rc;
	}

	return avc_has_perm(&selinux_state,
			    fromsid, tosid, SECCLASS_BINDER, BINDER__CALL,
			    NULL);
}