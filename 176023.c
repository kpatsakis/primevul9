void mremap_userfaultfd_complete(struct vm_userfaultfd_ctx *vm_ctx,
				 unsigned long from, unsigned long to,
				 unsigned long len)
{
	struct userfaultfd_ctx *ctx = vm_ctx->ctx;
	struct userfaultfd_wait_queue ewq;

	if (!ctx)
		return;

	if (to & ~PAGE_MASK) {
		userfaultfd_ctx_put(ctx);
		return;
	}

	msg_init(&ewq.msg);

	ewq.msg.event = UFFD_EVENT_REMAP;
	ewq.msg.arg.remap.from = from;
	ewq.msg.arg.remap.to = to;
	ewq.msg.arg.remap.len = len;

	userfaultfd_event_wait_completion(ctx, &ewq);
}