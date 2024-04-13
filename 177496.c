rndr_newbuf(struct sd_markdown *rndr, int type)
{
	static const size_t buf_size[2] = {256, 64};
	struct buf *work = NULL;
	struct stack *pool = &rndr->work_bufs[type];

	if (pool->size < pool->asize &&
		pool->item[pool->size] != NULL) {
		work = pool->item[pool->size++];
		work->size = 0;
	} else {
		work = bufnew(buf_size[type]);
		redcarpet_stack_push(pool, work);
	}

	return work;
}