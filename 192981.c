static void snd_seq_info_dump_subscribers(struct snd_info_buffer *buffer,
					  struct snd_seq_port_subs_info *group,
					  int is_src, char *msg)
{
	struct list_head *p;
	struct snd_seq_subscribers *s;
	int count = 0;

	down_read(&group->list_mutex);
	if (list_empty(&group->list_head)) {
		up_read(&group->list_mutex);
		return;
	}
	snd_iprintf(buffer, msg);
	list_for_each(p, &group->list_head) {
		if (is_src)
			s = list_entry(p, struct snd_seq_subscribers, src_list);
		else
			s = list_entry(p, struct snd_seq_subscribers, dest_list);
		if (count++)
			snd_iprintf(buffer, ", ");
		snd_iprintf(buffer, "%d:%d",
			    is_src ? s->info.dest.client : s->info.sender.client,
			    is_src ? s->info.dest.port : s->info.sender.port);
		if (s->info.flags & SNDRV_SEQ_PORT_SUBS_TIMESTAMP)
			snd_iprintf(buffer, "[%c:%d]", ((s->info.flags & SNDRV_SEQ_PORT_SUBS_TIME_REAL) ? 'r' : 't'), s->info.queue);
		if (group->exclusive)
			snd_iprintf(buffer, "[ex]");
	}
	up_read(&group->list_mutex);
	snd_iprintf(buffer, "\n");
}