static int do_video_get_event(unsigned int fd, unsigned int cmd,
		struct compat_video_event __user *up)
{
	struct video_event kevent;
	mm_segment_t old_fs = get_fs();
	int err;

	set_fs(KERNEL_DS);
	err = sys_ioctl(fd, cmd, (unsigned long) &kevent);
	set_fs(old_fs);

	if (!err) {
		err  = put_user(kevent.type, &up->type);
		err |= put_user(kevent.timestamp, &up->timestamp);
		err |= put_user(kevent.u.size.w, &up->u.size.w);
		err |= put_user(kevent.u.size.h, &up->u.size.h);
		err |= put_user(kevent.u.size.aspect_ratio,
				&up->u.size.aspect_ratio);
		if (err)
			err = -EFAULT;
	}

	return err;
}