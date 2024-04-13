static inline int snd_seq_write_pool_allocated(struct snd_seq_client *client)
{
	return snd_seq_total_cells(client->pool) > 0;
}