static int show_map(struct seq_file *m, void *v)
{
	struct proc_maps_private *priv = m->private;
	struct task_struct *task = priv->task;
	struct vm_area_struct *vma = v;
	struct mm_struct *mm = vma->vm_mm;
	struct file *file = vma->vm_file;
	int flags = vma->vm_flags;
	unsigned long ino = 0;
	dev_t dev = 0;
	int len;

	if (maps_protect && !ptrace_may_attach(task))
		return -EACCES;

	if (file) {
		struct inode *inode = vma->vm_file->f_path.dentry->d_inode;
		dev = inode->i_sb->s_dev;
		ino = inode->i_ino;
	}

	seq_printf(m, "%08lx-%08lx %c%c%c%c %08lx %02x:%02x %lu %n",
			vma->vm_start,
			vma->vm_end,
			flags & VM_READ ? 'r' : '-',
			flags & VM_WRITE ? 'w' : '-',
			flags & VM_EXEC ? 'x' : '-',
			flags & VM_MAYSHARE ? 's' : 'p',
			vma->vm_pgoff << PAGE_SHIFT,
			MAJOR(dev), MINOR(dev), ino, &len);

	/*
	 * Print the dentry name for named mappings, and a
	 * special [heap] marker for the heap:
	 */
	if (file) {
		pad_len_spaces(m, len);
		seq_path(m, file->f_path.mnt, file->f_path.dentry, "\n");
	} else {
		const char *name = arch_vma_name(vma);
		if (!name) {
			if (mm) {
				if (vma->vm_start <= mm->start_brk &&
						vma->vm_end >= mm->brk) {
					name = "[heap]";
				} else if (vma->vm_start <= mm->start_stack &&
					   vma->vm_end >= mm->start_stack) {
					name = "[stack]";
				}
			} else {
				name = "[vdso]";
			}
		}
		if (name) {
			pad_len_spaces(m, len);
			seq_puts(m, name);
		}
	}
	seq_putc(m, '\n');

	if (m->count < m->size)  /* vma is copied successfully */
		m->version = (vma != get_gate_vma(task))? vma->vm_start: 0;
	return 0;
}