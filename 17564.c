static int snd_ctl_remove_user_ctl(struct snd_ctl_file * file,
				   struct snd_ctl_elem_id *id)
{
	struct snd_card *card = file->card;
	struct snd_kcontrol *kctl;
	int idx, ret;

	down_write(&card->controls_rwsem);
	kctl = snd_ctl_find_id(card, id);
	if (kctl == NULL) {
		ret = -ENOENT;
		goto error;
	}
	if (!(kctl->vd[0].access & SNDRV_CTL_ELEM_ACCESS_USER)) {
		ret = -EINVAL;
		goto error;
	}
	for (idx = 0; idx < kctl->count; idx++)
		if (kctl->vd[idx].owner != NULL && kctl->vd[idx].owner != file) {
			ret = -EBUSY;
			goto error;
		}
	ret = snd_ctl_remove(card, kctl);
error:
	up_write(&card->controls_rwsem);
	return ret;
}