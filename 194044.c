void blk_flush_plug_list(struct blk_plug *plug, bool from_schedule)
{
	flush_plug_callbacks(plug, from_schedule);

	if (!list_empty(&plug->mq_list))
		blk_mq_flush_plug_list(plug, from_schedule);
}