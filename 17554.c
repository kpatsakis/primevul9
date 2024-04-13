static int snd_ctl_find_hole(struct snd_card *card, unsigned int count)
{
	unsigned int iter = 100000;

	while (snd_ctl_remove_numid_conflict(card, count)) {
		if (--iter == 0) {
			/* this situation is very unlikely */
			dev_err(card->dev, "unable to allocate new control numid\n");
			return -ENOMEM;
		}
	}
	return 0;
}