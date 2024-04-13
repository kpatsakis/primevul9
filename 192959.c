static int snd_seq_release(struct inode *inode, struct file *file)
{
	struct snd_seq_client *client = file->private_data;

	if (client) {
		seq_free_client(client);
		if (client->data.user.fifo)
			snd_seq_fifo_delete(&client->data.user.fifo);
		put_pid(client->data.user.owner);
		kfree(client);
	}

	return 0;
}