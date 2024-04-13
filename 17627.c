static int snd_ctl_elem_add_user(struct snd_ctl_file *file,
				 struct snd_ctl_elem_info __user *_info, int replace)
{
	struct snd_ctl_elem_info info;
	int err;

	if (copy_from_user(&info, _info, sizeof(info)))
		return -EFAULT;
	err = snd_ctl_elem_add(file, &info, replace);
	if (err < 0)
		return err;
	if (copy_to_user(_info, &info, sizeof(info))) {
		snd_ctl_remove_user_ctl(file, &info.id);
		return -EFAULT;
	}

	return 0;
}