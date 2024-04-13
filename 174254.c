static int selinux_task_movememory(struct task_struct *p)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid(p), SECCLASS_PROCESS,
			    PROCESS__SETSCHED, NULL);
}