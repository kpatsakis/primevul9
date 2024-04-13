static int snd_ctl_elem_info_user(struct snd_ctl_file *ctl,
				  struct snd_ctl_elem_info __user *_info)
{
	struct snd_ctl_elem_info info;
	int result;

	if (copy_from_user(&info, _info, sizeof(info)))
		return -EFAULT;
	result = snd_ctl_elem_info(ctl, &info);
	if (result < 0)
		return result;
	/* drop internal access flags */
	info.access &= ~(SNDRV_CTL_ELEM_ACCESS_SKIP_CHECK|
			 SNDRV_CTL_ELEM_ACCESS_LED_MASK);
	if (copy_to_user(_info, &info, sizeof(info)))
		return -EFAULT;
	return result;
}