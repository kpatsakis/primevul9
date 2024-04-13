static int selinux_file_send_sigiotask(struct task_struct *tsk,
				       struct fown_struct *fown, int signum)
{
	struct file *file;
	u32 sid = task_sid(tsk);
	u32 perm;
	struct file_security_struct *fsec;

	/* struct fown_struct is never outside the context of a struct file */
	file = container_of(fown, struct file, f_owner);

	fsec = selinux_file(file);

	if (!signum)
		perm = signal_to_av(SIGIO); /* as per send_sigio_to_task */
	else
		perm = signal_to_av(signum);

	return avc_has_perm(&selinux_state,
			    fsec->fown_sid, sid,
			    SECCLASS_PROCESS, perm, NULL);
}