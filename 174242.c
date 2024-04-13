static int selinux_capget(struct task_struct *target, kernel_cap_t *effective,
			  kernel_cap_t *inheritable, kernel_cap_t *permitted)
{
	return avc_has_perm(&selinux_state,
			    current_sid(), task_sid(target), SECCLASS_PROCESS,
			    PROCESS__GETCAP, NULL);
}