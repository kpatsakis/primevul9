int snd_seq_kernel_client_enqueue(int client, struct snd_seq_event * ev,
				  int atomic, int hop)
{
	return kernel_client_enqueue(client, ev, NULL, 0, atomic, hop);
}