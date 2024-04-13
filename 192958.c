int snd_seq_delete_kernel_client(int client)
{
	struct snd_seq_client *ptr;

	if (snd_BUG_ON(in_interrupt()))
		return -EBUSY;

	ptr = clientptr(client);
	if (ptr == NULL)
		return -EINVAL;

	seq_free_client(ptr);
	kfree(ptr);
	return 0;
}