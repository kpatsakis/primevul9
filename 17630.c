void snd_ctl_disconnect_layer(struct snd_ctl_layer_ops *lops)
{
	struct snd_ctl_layer_ops *lops2, *prev_lops2;

	down_write(&snd_ctl_layer_rwsem);
	for (lops2 = snd_ctl_layer, prev_lops2 = NULL; lops2; lops2 = lops2->next) {
		if (lops2 == lops) {
			if (!prev_lops2)
				snd_ctl_layer = lops->next;
			else
				prev_lops2->next = lops->next;
			break;
		}
		prev_lops2 = lops2;
	}
	up_write(&snd_ctl_layer_rwsem);
}