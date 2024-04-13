static inline struct uffd_msg userfault_msg(unsigned long address,
					    unsigned int flags,
					    unsigned long reason,
					    unsigned int features)
{
	struct uffd_msg msg;
	msg_init(&msg);
	msg.event = UFFD_EVENT_PAGEFAULT;
	msg.arg.pagefault.address = address;
	if (flags & FAULT_FLAG_WRITE)
		/*
		 * If UFFD_FEATURE_PAGEFAULT_FLAG_WP was set in the
		 * uffdio_api.features and UFFD_PAGEFAULT_FLAG_WRITE
		 * was not set in a UFFD_EVENT_PAGEFAULT, it means it
		 * was a read fault, otherwise if set it means it's
		 * a write fault.
		 */
		msg.arg.pagefault.flags |= UFFD_PAGEFAULT_FLAG_WRITE;
	if (reason & VM_UFFD_WP)
		/*
		 * If UFFD_FEATURE_PAGEFAULT_FLAG_WP was set in the
		 * uffdio_api.features and UFFD_PAGEFAULT_FLAG_WP was
		 * not set in a UFFD_EVENT_PAGEFAULT, it means it was
		 * a missing fault, otherwise if set it means it's a
		 * write protect fault.
		 */
		msg.arg.pagefault.flags |= UFFD_PAGEFAULT_FLAG_WP;
	if (features & UFFD_FEATURE_THREAD_ID)
		msg.arg.pagefault.feat.ptid = task_pid_vnr(current);
	return msg;
}