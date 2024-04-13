static int multicast_event(struct snd_seq_client *client, struct snd_seq_event *event,
			   int atomic, int hop)
{
	pr_debug("ALSA: seq: multicast not supported yet.\n");
	return 0; /* ignored */
}