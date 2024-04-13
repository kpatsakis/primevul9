static __always_inline long __get_user_pages_locked(struct task_struct *tsk,
						struct mm_struct *mm,
						unsigned long start,
						unsigned long nr_pages,
						struct page **pages,
						struct vm_area_struct **vmas,
						int *locked,
						unsigned int flags)
{
	long ret, pages_done;
	bool lock_dropped;

	if (locked) {
		/* if VM_FAULT_RETRY can be returned, vmas become invalid */
		BUG_ON(vmas);
		/* check caller initialized locked */
		BUG_ON(*locked != 1);
	}

	/*
	 * FOLL_PIN and FOLL_GET are mutually exclusive. Traditional behavior
	 * is to set FOLL_GET if the caller wants pages[] filled in (but has
	 * carelessly failed to specify FOLL_GET), so keep doing that, but only
	 * for FOLL_GET, not for the newer FOLL_PIN.
	 *
	 * FOLL_PIN always expects pages to be non-null, but no need to assert
	 * that here, as any failures will be obvious enough.
	 */
	if (pages && !(flags & FOLL_PIN))
		flags |= FOLL_GET;

	pages_done = 0;
	lock_dropped = false;
	for (;;) {
		ret = __get_user_pages(tsk, mm, start, nr_pages, flags, pages,
				       vmas, locked);
		if (!locked)
			/* VM_FAULT_RETRY couldn't trigger, bypass */
			return ret;

		/* VM_FAULT_RETRY cannot return errors */
		if (!*locked) {
			BUG_ON(ret < 0);
			BUG_ON(ret >= nr_pages);
		}

		if (ret > 0) {
			nr_pages -= ret;
			pages_done += ret;
			if (!nr_pages)
				break;
		}
		if (*locked) {
			/*
			 * VM_FAULT_RETRY didn't trigger or it was a
			 * FOLL_NOWAIT.
			 */
			if (!pages_done)
				pages_done = ret;
			break;
		}
		/*
		 * VM_FAULT_RETRY triggered, so seek to the faulting offset.
		 * For the prefault case (!pages) we only update counts.
		 */
		if (likely(pages))
			pages += ret;
		start += ret << PAGE_SHIFT;
		lock_dropped = true;

retry:
		/*
		 * Repeat on the address that fired VM_FAULT_RETRY
		 * with both FAULT_FLAG_ALLOW_RETRY and
		 * FAULT_FLAG_TRIED.  Note that GUP can be interrupted
		 * by fatal signals, so we need to check it before we
		 * start trying again otherwise it can loop forever.
		 */

		if (fatal_signal_pending(current)) {
			if (!pages_done)
				pages_done = -EINTR;
			break;
		}

		ret = down_read_killable(&mm->mmap_sem);
		if (ret) {
			BUG_ON(ret > 0);
			if (!pages_done)
				pages_done = ret;
			break;
		}

		*locked = 1;
		ret = __get_user_pages(tsk, mm, start, 1, flags | FOLL_TRIED,
				       pages, NULL, locked);
		if (!*locked) {
			/* Continue to retry until we succeeded */
			BUG_ON(ret != 0);
			goto retry;
		}
		if (ret != 1) {
			BUG_ON(ret > 1);
			if (!pages_done)
				pages_done = ret;
			break;
		}
		nr_pages--;
		pages_done++;
		if (!nr_pages)
			break;
		if (likely(pages))
			pages++;
		start += PAGE_SIZE;
	}
	if (lock_dropped && *locked) {
		/*
		 * We must let the caller know we temporarily dropped the lock
		 * and so the critical section protected by it was lost.
		 */
		up_read(&mm->mmap_sem);
		*locked = 0;
	}
	return pages_done;
}