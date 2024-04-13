void blk_finish_plug(struct blk_plug *plug)
{
	if (plug != current->plug)
		return;
	blk_flush_plug_list(plug, false);

	current->plug = NULL;
}