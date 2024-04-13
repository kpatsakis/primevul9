static void netlink_walk_stop(struct nl_seq_iter *iter)
{
	rhashtable_walk_stop(&iter->hti);
	rhashtable_walk_exit(&iter->hti);
}