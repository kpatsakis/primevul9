static int snd_seq_ioctl_pversion(struct snd_seq_client *client, void *arg)
{
	int *pversion = arg;

	*pversion = SNDRV_SEQ_VERSION;
	return 0;
}