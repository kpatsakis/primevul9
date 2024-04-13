static void *__netlink_seq_next(struct seq_file *seq)
{
	struct nl_seq_iter *iter = seq->private;
	struct netlink_sock *nlk;

	do {
		for (;;) {
			int err;

			nlk = rhashtable_walk_next(&iter->hti);

			if (IS_ERR(nlk)) {
				if (PTR_ERR(nlk) == -EAGAIN)
					continue;

				return nlk;
			}

			if (nlk)
				break;

			netlink_walk_stop(iter);
			if (++iter->link >= MAX_LINKS)
				return NULL;

			err = netlink_walk_start(iter);
			if (err)
				return ERR_PTR(err);
		}
	} while (sock_net(&nlk->sk) != seq_file_net(seq));

	return nlk;
}