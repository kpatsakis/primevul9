rndr_popbuf(struct sd_markdown *rndr, int type)
{
	rndr->work_bufs[type].size--;
}