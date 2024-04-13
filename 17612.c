static int snd_ctl_elem_info(struct snd_ctl_file *ctl,
			     struct snd_ctl_elem_info *info)
{
	struct snd_card *card = ctl->card;
	struct snd_kcontrol *kctl;
	int result;

	down_read(&card->controls_rwsem);
	kctl = snd_ctl_find_id(card, &info->id);
	if (kctl == NULL)
		result = -ENOENT;
	else
		result = __snd_ctl_elem_info(card, kctl, info, ctl);
	up_read(&card->controls_rwsem);
	return result;
}