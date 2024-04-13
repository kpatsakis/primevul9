static int selinux_task_getioprio(struct task_struct *p)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid(p), SECCLASS_PROCESS,
			    PROCESS__GETSCHED, NULL);
}