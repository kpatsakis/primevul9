int snd_seq_kernel_client_enqueue_blocking(int client, struct snd_seq_event * ev,
					   struct file *file,
					   int atomic, int hop)
{
	return kernel_client_enqueue(client, ev, file, 1, atomic, hop);
}