struct snd_kcontrol *snd_ctl_find_numid(struct snd_card *card, unsigned int numid)
{
	if (snd_BUG_ON(!card || !numid))
		return NULL;
#ifdef CONFIG_SND_CTL_FAST_LOOKUP
	return xa_load(&card->ctl_numids, numid);
#else
	return snd_ctl_find_numid_slow(card, numid);
#endif
}