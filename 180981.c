static void free_chmap(struct snd_pcm_str *pstr)
{
	if (pstr->chmap_kctl) {
		snd_ctl_remove(pstr->pcm->card, pstr->chmap_kctl);
		pstr->chmap_kctl = NULL;
	}
}