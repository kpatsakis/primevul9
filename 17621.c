static int _snd_ctl_register_ioctl(snd_kctl_ioctl_func_t fcn, struct list_head *lists)
{
	struct snd_kctl_ioctl *pn;

	pn = kzalloc(sizeof(struct snd_kctl_ioctl), GFP_KERNEL);
	if (pn == NULL)
		return -ENOMEM;
	pn->fioctl = fcn;
	down_write(&snd_ioctl_rwsem);
	list_add_tail(&pn->list, lists);
	up_write(&snd_ioctl_rwsem);
	return 0;
}