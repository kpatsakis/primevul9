static int i40e_get_max_queues_for_channel(struct i40e_vsi *vsi)
{
	struct i40e_channel *ch, *ch_tmp;
	int max = 0;

	list_for_each_entry_safe(ch, ch_tmp, &vsi->ch_list, list) {
		if (!ch->initialized)
			continue;
		if (ch->num_queue_pairs > max)
			max = ch->num_queue_pairs;
	}

	return max;
}