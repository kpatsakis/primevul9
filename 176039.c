static inline bool userfaultfd_huge_must_wait(struct userfaultfd_ctx *ctx,
					 struct vm_area_struct *vma,
					 unsigned long address,
					 unsigned long flags,
					 unsigned long reason)
{
	return false;	/* should never get here */
}