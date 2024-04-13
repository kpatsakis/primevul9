static void dup_fctx(struct userfaultfd_fork_ctx *fctx)
{
	struct userfaultfd_ctx *ctx = fctx->orig;
	struct userfaultfd_wait_queue ewq;

	msg_init(&ewq.msg);

	ewq.msg.event = UFFD_EVENT_FORK;
	ewq.msg.arg.reserved.reserved1 = (unsigned long)fctx->new;

	userfaultfd_event_wait_completion(ctx, &ewq);
}