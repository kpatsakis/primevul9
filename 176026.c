static __always_inline int validate_range(struct mm_struct *mm,
					  __u64 start, __u64 len)
{
	__u64 task_size = mm->task_size;

	if (start & ~PAGE_MASK)
		return -EINVAL;
	if (len & ~PAGE_MASK)
		return -EINVAL;
	if (!len)
		return -EINVAL;
	if (start < mmap_min_addr)
		return -EINVAL;
	if (start >= task_size)
		return -EINVAL;
	if (len > task_size - start)
		return -EINVAL;
	return 0;
}