int vm_munmap(unsigned long start, size_t len)
{
	int ret;
	struct mm_struct *mm = current->mm;
	LIST_HEAD(uf);

	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;

	ret = do_munmap(mm, start, len, &uf);
	up_write(&mm->mmap_sem);
	userfaultfd_unmap_complete(mm, &uf);
	return ret;
}