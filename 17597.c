int snd_ctl_activate_id(struct snd_card *card, struct snd_ctl_elem_id *id,
			int active)
{
	struct snd_kcontrol *kctl;
	struct snd_kcontrol_volatile *vd;
	unsigned int index_offset;
	int ret;

	down_write(&card->controls_rwsem);
	kctl = snd_ctl_find_id(card, id);
	if (kctl == NULL) {
		ret = -ENOENT;
		goto unlock;
	}
	index_offset = snd_ctl_get_ioff(kctl, id);
	vd = &kctl->vd[index_offset];
	ret = 0;
	if (active) {
		if (!(vd->access & SNDRV_CTL_ELEM_ACCESS_INACTIVE))
			goto unlock;
		vd->access &= ~SNDRV_CTL_ELEM_ACCESS_INACTIVE;
	} else {
		if (vd->access & SNDRV_CTL_ELEM_ACCESS_INACTIVE)
			goto unlock;
		vd->access |= SNDRV_CTL_ELEM_ACCESS_INACTIVE;
	}
	snd_ctl_build_ioff(id, kctl, index_offset);
	downgrade_write(&card->controls_rwsem);
	snd_ctl_notify_one(card, SNDRV_CTL_EVENT_MASK_INFO, kctl, index_offset);
	up_read(&card->controls_rwsem);
	return 1;

 unlock:
	up_write(&card->controls_rwsem);
	return ret;
}