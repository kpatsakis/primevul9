void snd_ctl_register_layer(struct snd_ctl_layer_ops *lops)
{
	struct snd_card *card;
	int card_number;

	down_write(&snd_ctl_layer_rwsem);
	lops->next = snd_ctl_layer;
	snd_ctl_layer = lops;
	up_write(&snd_ctl_layer_rwsem);
	for (card_number = 0; card_number < SNDRV_CARDS; card_number++) {
		card = snd_card_ref(card_number);
		if (card) {
			down_read(&card->controls_rwsem);
			lops->lregister(card);
			up_read(&card->controls_rwsem);
			snd_card_unref(card);
		}
	}
}