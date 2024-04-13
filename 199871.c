static void compat_ioctl_error(struct file *filp, unsigned int fd,
		unsigned int cmd, unsigned long arg)
{
	char buf[10];
	char *fn = "?";
	char *path;

	/* find the name of the device. */
	path = (char *)__get_free_page(GFP_KERNEL);
	if (path) {
		fn = d_path(filp->f_dentry, filp->f_vfsmnt, path, PAGE_SIZE);
		if (IS_ERR(fn))
			fn = "?";
	}

	sprintf(buf,"'%c'", (cmd>>24) & 0x3f);
	if (!isprint(buf[1]))
		sprintf(buf, "%02x", buf[1]);
	compat_printk("ioctl32(%s:%d): Unknown cmd fd(%d) "
			"cmd(%08x){%s} arg(%08x) on %s\n",
			current->comm, current->pid,
			(int)fd, (unsigned int)cmd, buf,
			(unsigned int)arg, fn);

	if (path)
		free_page((unsigned long)path);
}