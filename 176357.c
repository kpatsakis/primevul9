static struct page *no_page_table(struct vm_area_struct *vma,
		unsigned int flags)
{
	/*
	 * When core dumping an enormous anonymous area that nobody
	 * has touched so far, we don't want to allocate unnecessary pages or
	 * page tables.  Return error instead of NULL to skip handle_mm_fault,
	 * then get_dump_page() will return NULL to leave a hole in the dump.
	 * But we can only make this optimization where a hole would surely
	 * be zero-filled if handle_mm_fault() actually did handle it.
	 */
	if ((flags & FOLL_DUMP) &&
			(vma_is_anonymous(vma) || !vma->vm_ops->fault))
		return ERR_PTR(-EFAULT);
	return NULL;
}