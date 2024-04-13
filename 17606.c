static int __snd_ctl_elem_info(struct snd_card *card,
			       struct snd_kcontrol *kctl,
			       struct snd_ctl_elem_info *info,
			       struct snd_ctl_file *ctl)
{
	struct snd_kcontrol_volatile *vd;
	unsigned int index_offset;
	int result;

#ifdef CONFIG_SND_DEBUG
	info->access = 0;
#endif
	result = snd_power_ref_and_wait(card);
	if (!result)
		result = kctl->info(kctl, info);
	snd_power_unref(card);
	if (result >= 0) {
		snd_BUG_ON(info->access);
		index_offset = snd_ctl_get_ioff(kctl, &info->id);
		vd = &kctl->vd[index_offset];
		snd_ctl_build_ioff(&info->id, kctl, index_offset);
		info->access = vd->access;
		if (vd->owner) {
			info->access |= SNDRV_CTL_ELEM_ACCESS_LOCK;
			if (vd->owner == ctl)
				info->access |= SNDRV_CTL_ELEM_ACCESS_OWNER;
			info->owner = pid_vnr(vd->owner->pid);
		} else {
			info->owner = -1;
		}
		if (!snd_ctl_skip_validation(info) &&
		    snd_ctl_check_elem_info(card, info) < 0)
			result = -EINVAL;
	}
	return result;
}