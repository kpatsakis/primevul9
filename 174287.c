static int selinux_binder_set_context_mgr(struct task_struct *mgr)
{
	u32 mysid = current_sid();
	u32 mgrsid = task_sid(mgr);

	return avc_has_perm(&selinux_state,
			    mysid, mgrsid, SECCLASS_BINDER,
			    BINDER__SET_CONTEXT_MGR, NULL);
}