void userfaultfd_unmap_complete(struct mm_struct *mm, struct list_head *uf)
{
	struct userfaultfd_unmap_ctx *ctx, *n;
	struct userfaultfd_wait_queue ewq;

	list_for_each_entry_safe(ctx, n, uf, list) {
		msg_init(&ewq.msg);

		ewq.msg.event = UFFD_EVENT_UNMAP;
		ewq.msg.arg.remove.start = ctx->start;
		ewq.msg.arg.remove.end = ctx->end;

		userfaultfd_event_wait_completion(ctx->ctx, &ewq);

		list_del(&ctx->list);
		kfree(ctx);
	}
}