void dup_userfaultfd_complete(struct list_head *fcs)
{
	struct userfaultfd_fork_ctx *fctx, *n;

	list_for_each_entry_safe(fctx, n, fcs, list) {
		dup_fctx(fctx);
		list_del(&fctx->list);
		kfree(fctx);
	}
}