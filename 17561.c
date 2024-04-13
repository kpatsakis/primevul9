void snd_ctl_notify_one(struct snd_card *card, unsigned int mask,
			struct snd_kcontrol *kctl, unsigned int ioff)
{
	struct snd_ctl_elem_id id = kctl->id;
	struct snd_ctl_layer_ops *lops;

	id.index += ioff;
	id.numid += ioff;
	snd_ctl_notify(card, mask, &id);
	down_read(&snd_ctl_layer_rwsem);
	for (lops = snd_ctl_layer; lops; lops = lops->next)
		lops->lnotify(card, mask, kctl, ioff);
	up_read(&snd_ctl_layer_rwsem);
}