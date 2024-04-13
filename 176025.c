bool userfaultfd_remove(struct vm_area_struct *vma,
			unsigned long start, unsigned long end)
{
	struct mm_struct *mm = vma->vm_mm;
	struct userfaultfd_ctx *ctx;
	struct userfaultfd_wait_queue ewq;

	ctx = vma->vm_userfaultfd_ctx.ctx;
	if (!ctx || !(ctx->features & UFFD_FEATURE_EVENT_REMOVE))
		return true;

	userfaultfd_ctx_get(ctx);
	up_read(&mm->mmap_sem);

	msg_init(&ewq.msg);

	ewq.msg.event = UFFD_EVENT_REMOVE;
	ewq.msg.arg.remove.start = start;
	ewq.msg.arg.remove.end = end;

	userfaultfd_event_wait_completion(ctx, &ewq);

	return false;
}