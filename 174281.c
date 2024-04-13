static int selinux_ptrace_access_check(struct task_struct *child,
				     unsigned int mode)
{
	u32 sid = current_sid();
	u32 csid = task_sid(child);

	if (mode & PTRACE_MODE_READ)
		return avc_has_perm(&selinux_state,
				    sid, csid, SECCLASS_FILE, FILE__READ, NULL);

	return avc_has_perm(&selinux_state,
			    sid, csid, SECCLASS_PROCESS, PROCESS__PTRACE, NULL);
}