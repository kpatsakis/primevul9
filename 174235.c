static int selinux_ptrace_traceme(struct task_struct *parent)
{
	return avc_has_perm(&selinux_state,
			    task_sid(parent), current_sid(), SECCLASS_PROCESS,
			    PROCESS__PTRACE, NULL);
}