static struct page *get_arg_page(struct linux_binprm *bprm, unsigned long pos,
		int write)
{
	struct page *page;
	int ret;
	unsigned int gup_flags = FOLL_FORCE;

#ifdef CONFIG_STACK_GROWSUP
	if (write) {
		ret = expand_downwards(bprm->vma, pos);
		if (ret < 0)
			return NULL;
	}
#endif

	if (write)
		gup_flags |= FOLL_WRITE;

	/*
	 * We are doing an exec().  'current' is the process
	 * doing the exec and bprm->mm is the new process's mm.
	 */
	ret = get_user_pages_remote(current, bprm->mm, pos, 1, gup_flags,
			&page, NULL, NULL);
	if (ret <= 0)
		return NULL;

	if (write) {
		unsigned long size = bprm->vma->vm_end - bprm->vma->vm_start;
		unsigned long ptr_size, limit;

		/*
		 * Since the stack will hold pointers to the strings, we
		 * must account for them as well.
		 *
		 * The size calculation is the entire vma while each arg page is
		 * built, so each time we get here it's calculating how far it
		 * is currently (rather than each call being just the newly
		 * added size from the arg page).  As a result, we need to
		 * always add the entire size of the pointers, so that on the
		 * last call to get_arg_page() we'll actually have the entire
		 * correct size.
		 */
		ptr_size = (bprm->argc + bprm->envc) * sizeof(void *);
		if (ptr_size > ULONG_MAX - size)
			goto fail;
		size += ptr_size;

		acct_arg_size(bprm, size / PAGE_SIZE);

		/*
		 * We've historically supported up to 32 pages (ARG_MAX)
		 * of argument strings even with small stacks
		 */
		if (size <= ARG_MAX)
			return page;

		/*
		 * Limit to 1/4 of the max stack size or 3/4 of _STK_LIM
		 * (whichever is smaller) for the argv+env strings.
		 * This ensures that:
		 *  - the remaining binfmt code will not run out of stack space,
		 *  - the program will have a reasonable amount of stack left
		 *    to work from.
		 */
		limit = _STK_LIM / 4 * 3;
		limit = min(limit, rlimit(RLIMIT_STACK) / 4);
		if (size > limit)
			goto fail;
	}

	return page;

fail:
	put_page(page);
	return NULL;
}