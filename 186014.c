static int do_video_stillpicture(unsigned int fd, unsigned int cmd,
	struct compat_video_still_picture __user *up)
{
	struct video_still_picture __user *up_native;
	compat_uptr_t fp;
	int32_t size;
	int err;

	err  = get_user(fp, &up->iFrame);
	err |= get_user(size, &up->size);
	if (err)
		return -EFAULT;

	up_native =
		compat_alloc_user_space(sizeof(struct video_still_picture));

	err =  put_user(compat_ptr(fp), &up_native->iFrame);
	err |= put_user(size, &up_native->size);
	if (err)
		return -EFAULT;

	err = sys_ioctl(fd, cmd, (unsigned long) up_native);

	return err;
}