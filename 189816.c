static ssize_t clear_refs_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct task_struct *task;
	char buffer[PROC_NUMBUF], *end;
	struct mm_struct *mm;
	struct vm_area_struct *vma;

	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count))
		return -EFAULT;
	if (!simple_strtol(buffer, &end, 0))
		return -EINVAL;
	if (*end == '\n')
		end++;
	task = get_proc_task(file->f_path.dentry->d_inode);
	if (!task)
		return -ESRCH;
	mm = get_task_mm(task);
	if (mm) {
		down_read(&mm->mmap_sem);
		for (vma = mm->mmap; vma; vma = vma->vm_next)
			if (!is_vm_hugetlb_page(vma))
				walk_page_range(mm, vma->vm_start, vma->vm_end,
						&clear_refs_walk, vma);
		flush_tlb_mm(mm);
		up_read(&mm->mmap_sem);
		mmput(mm);
	}
	put_task_struct(task);
	if (end - buffer == 0)
		return -EIO;
	return end - buffer;
}