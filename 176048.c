static bool has_unmap_ctx(struct userfaultfd_ctx *ctx, struct list_head *unmaps,
			  unsigned long start, unsigned long end)
{
	struct userfaultfd_unmap_ctx *unmap_ctx;

	list_for_each_entry(unmap_ctx, unmaps, list)
		if (unmap_ctx->ctx == ctx && unmap_ctx->start == start &&
		    unmap_ctx->end == end)
			return true;

	return false;
}