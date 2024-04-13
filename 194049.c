static void flush_plug_callbacks(struct blk_plug *plug, bool from_schedule)
{
	LIST_HEAD(callbacks);

	while (!list_empty(&plug->cb_list)) {
		list_splice_init(&plug->cb_list, &callbacks);

		while (!list_empty(&callbacks)) {
			struct blk_plug_cb *cb = list_first_entry(&callbacks,
							  struct blk_plug_cb,
							  list);
			list_del(&cb->list);
			cb->callback(cb, from_schedule);
		}
	}
}