static struct kiocb *lookup_kiocb(struct kioctx *ctx, struct iocb __user *iocb,
				  u32 key)
{
	struct list_head *pos;

	assert_spin_locked(&ctx->ctx_lock);

	if (key != KIOCB_KEY)
		return NULL;

	/* TODO: use a hash or array, this sucks. */
	list_for_each(pos, &ctx->active_reqs) {
		struct kiocb *kiocb = list_kiocb(pos);
		if (kiocb->ki_obj.user == iocb)
			return kiocb;
	}
	return NULL;
}