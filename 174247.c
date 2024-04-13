static int selinux_binder_transfer_binder(struct task_struct *from,
					  struct task_struct *to)
{
	u32 fromsid = task_sid(from);
	u32 tosid = task_sid(to);

	return avc_has_perm(&selinux_state,
			    fromsid, tosid, SECCLASS_BINDER, BINDER__TRANSFER,
			    NULL);
}