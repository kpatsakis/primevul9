sd_markdown_free(struct sd_markdown *md)
{
	size_t i;

	for (i = 0; i < (size_t)md->work_bufs[BUFFER_SPAN].asize; ++i)
		bufrelease(md->work_bufs[BUFFER_SPAN].item[i]);

	for (i = 0; i < (size_t)md->work_bufs[BUFFER_BLOCK].asize; ++i)
		bufrelease(md->work_bufs[BUFFER_BLOCK].item[i]);

	redcarpet_stack_free(&md->work_bufs[BUFFER_SPAN]);
	redcarpet_stack_free(&md->work_bufs[BUFFER_BLOCK]);

	free(md);
}