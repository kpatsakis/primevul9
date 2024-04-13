static int netlink_walk_start(struct nl_seq_iter *iter)
{
	int err;

	err = rhashtable_walk_init(&nl_table[iter->link].hash, &iter->hti,
				   GFP_KERNEL);
	if (err) {
		iter->link = MAX_LINKS;
		return err;
	}

	err = rhashtable_walk_start(&iter->hti);
	return err == -EAGAIN ? 0 : err;
}