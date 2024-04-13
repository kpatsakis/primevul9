static int snd_ctl_elem_list_user(struct snd_card *card,
				  struct snd_ctl_elem_list __user *_list)
{
	struct snd_ctl_elem_list list;
	int err;

	if (copy_from_user(&list, _list, sizeof(list)))
		return -EFAULT;
	err = snd_ctl_elem_list(card, &list);
	if (err)
		return err;
	if (copy_to_user(_list, &list, sizeof(list)))
		return -EFAULT;

	return 0;
}