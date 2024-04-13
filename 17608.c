static int snd_ctl_new(struct snd_kcontrol **kctl, unsigned int count,
		       unsigned int access, struct snd_ctl_file *file)
{
	unsigned int idx;

	if (count == 0 || count > MAX_CONTROL_COUNT)
		return -EINVAL;

	*kctl = kzalloc(struct_size(*kctl, vd, count), GFP_KERNEL);
	if (!*kctl)
		return -ENOMEM;

	for (idx = 0; idx < count; idx++) {
		(*kctl)->vd[idx].access = access;
		(*kctl)->vd[idx].owner = file;
	}
	(*kctl)->count = count;

	return 0;
}