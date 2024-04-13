static int snd_seq_ioctl_query_subs(struct snd_seq_client *client, void *arg)
{
	struct snd_seq_query_subs *subs = arg;
	int result = -ENXIO;
	struct snd_seq_client *cptr = NULL;
	struct snd_seq_client_port *port = NULL;
	struct snd_seq_port_subs_info *group;
	struct list_head *p;
	int i;

	if ((cptr = snd_seq_client_use_ptr(subs->root.client)) == NULL)
		goto __end;
	if ((port = snd_seq_port_use_ptr(cptr, subs->root.port)) == NULL)
		goto __end;

	switch (subs->type) {
	case SNDRV_SEQ_QUERY_SUBS_READ:
		group = &port->c_src;
		break;
	case SNDRV_SEQ_QUERY_SUBS_WRITE:
		group = &port->c_dest;
		break;
	default:
		goto __end;
	}

	down_read(&group->list_mutex);
	/* search for the subscriber */
	subs->num_subs = group->count;
	i = 0;
	result = -ENOENT;
	list_for_each(p, &group->list_head) {
		if (i++ == subs->index) {
			/* found! */
			struct snd_seq_subscribers *s;
			if (subs->type == SNDRV_SEQ_QUERY_SUBS_READ) {
				s = list_entry(p, struct snd_seq_subscribers, src_list);
				subs->addr = s->info.dest;
			} else {
				s = list_entry(p, struct snd_seq_subscribers, dest_list);
				subs->addr = s->info.sender;
			}
			subs->flags = s->info.flags;
			subs->queue = s->info.queue;
			result = 0;
			break;
		}
	}
	up_read(&group->list_mutex);

      __end:
   	if (port)
		snd_seq_port_unlock(port);
	if (cptr)
		snd_seq_client_unlock(cptr);

	return result;
}