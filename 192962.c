static struct snd_seq_client *clientptr(int clientid)
{
	if (clientid < 0 || clientid >= SNDRV_SEQ_MAX_CLIENTS) {
		pr_debug("ALSA: seq: oops. Trying to get pointer to client %d\n",
			   clientid);
		return NULL;
	}
	return clienttab[clientid];
}