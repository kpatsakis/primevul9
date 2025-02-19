static int snd_ctl_elem_lock(struct snd_ctl_file *file,
			     struct snd_ctl_elem_id __user *_id)
{
	struct snd_card *card = file->card;
	struct snd_ctl_elem_id id;
	struct snd_kcontrol *kctl;
	struct snd_kcontrol_volatile *vd;
	int result;

	if (copy_from_user(&id, _id, sizeof(id)))
		return -EFAULT;
	down_write(&card->controls_rwsem);
	kctl = snd_ctl_find_id(card, &id);
	if (kctl == NULL) {
		result = -ENOENT;
	} else {
		vd = &kctl->vd[snd_ctl_get_ioff(kctl, &id)];
		if (vd->owner != NULL)
			result = -EBUSY;
		else {
			vd->owner = file;
			result = 0;
		}
	}
	up_write(&card->controls_rwsem);
	return result;
}