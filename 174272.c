static int selinux_task_kill(struct task_struct *p, struct kernel_siginfo *info,
				int sig, const struct cred *cred)
{
	u32 secid;
	u32 perm;

	if (!sig)
		perm = PROCESS__SIGNULL; /* null signal; existence test */
	else
		perm = signal_to_av(sig);
	if (!cred)
		secid = current_sid();
	else
		secid = cred_sid(cred);
	return avc_has_perm(&selinux_state,
			    secid, task_sid(p), SECCLASS_PROCESS, perm, NULL);
}