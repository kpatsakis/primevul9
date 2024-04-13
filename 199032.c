copy_shmid_from_user(struct shmid64_ds *out, void __user *buf, int version)
{
	switch (version) {
	case IPC_64:
		if (copy_from_user(out, buf, sizeof(*out)))
			return -EFAULT;
		return 0;
	case IPC_OLD:
	    {
		struct shmid_ds tbuf_old;

		if (copy_from_user(&tbuf_old, buf, sizeof(tbuf_old)))
			return -EFAULT;

		out->shm_perm.uid	= tbuf_old.shm_perm.uid;
		out->shm_perm.gid	= tbuf_old.shm_perm.gid;
		out->shm_perm.mode	= tbuf_old.shm_perm.mode;

		return 0;
	    }
	default:
		return -EINVAL;
	}
}