static int ppp_sock_fprog_ioctl_trans(unsigned int fd, unsigned int cmd,
			struct sock_fprog32 __user *u_fprog32)
{
	struct sock_fprog __user *u_fprog64 = compat_alloc_user_space(sizeof(struct sock_fprog));
	void __user *fptr64;
	u32 fptr32;
	u16 flen;

	if (get_user(flen, &u_fprog32->len) ||
	    get_user(fptr32, &u_fprog32->filter))
		return -EFAULT;

	fptr64 = compat_ptr(fptr32);

	if (put_user(flen, &u_fprog64->len) ||
	    put_user(fptr64, &u_fprog64->filter))
		return -EFAULT;

	if (cmd == PPPIOCSPASS32)
		cmd = PPPIOCSPASS;
	else
		cmd = PPPIOCSACTIVE;

	return sys_ioctl(fd, cmd, (unsigned long) u_fprog64);
}