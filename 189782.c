static unsigned mounts_poll(struct file *file, poll_table *wait)
{
	struct proc_mounts *p = file->private_data;
	struct mnt_namespace *ns = p->m.private;
	unsigned res = 0;

	poll_wait(file, &ns->poll, wait);

	spin_lock(&vfsmount_lock);
	if (p->event != ns->event) {
		p->event = ns->event;
		res = POLLERR;
	}
	spin_unlock(&vfsmount_lock);

	return res;
}