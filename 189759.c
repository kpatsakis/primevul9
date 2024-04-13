int proc_exe_link(struct inode *inode, struct dentry **dentry, struct vfsmount **mnt)
{
	struct vm_area_struct * vma;
	int result = -ENOENT;
	struct task_struct *task = get_proc_task(inode);
	struct mm_struct * mm = NULL;

	if (task) {
		mm = get_task_mm(task);
		put_task_struct(task);
	}
	if (!mm)
		goto out;
	down_read(&mm->mmap_sem);

	vma = mm->mmap;
	while (vma) {
		if ((vma->vm_flags & VM_EXECUTABLE) && vma->vm_file)
			break;
		vma = vma->vm_next;
	}

	if (vma) {
		*mnt = mntget(vma->vm_file->f_path.mnt);
		*dentry = dget(vma->vm_file->f_path.dentry);
		result = 0;
	}

	up_read(&mm->mmap_sem);
	mmput(mm);
out:
	return result;
}