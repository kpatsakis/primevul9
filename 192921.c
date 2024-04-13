int snd_seq_set_queue_tempo(int client, struct snd_seq_queue_tempo *tempo)
{
	if (!snd_seq_queue_check_access(tempo->queue, client))
		return -EPERM;
	return snd_seq_queue_timer_set_tempo(tempo->queue, client, tempo);
}