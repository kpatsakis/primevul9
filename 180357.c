static inline int is_mergeable_vma(struct vm_area_struct *vma,
				struct file *file, unsigned long vm_flags,
				struct vm_userfaultfd_ctx vm_userfaultfd_ctx)
{
	/*
	 * VM_SOFTDIRTY should not prevent from VMA merging, if we
	 * match the flags but dirty bit -- the caller should mark
	 * merged VMA as dirty. If dirty bit won't be excluded from
	 * comparison, we increase pressue on the memory system forcing
	 * the kernel to generate new VMAs when old one could be
	 * extended instead.
	 */
	if ((vma->vm_flags ^ vm_flags) & ~VM_SOFTDIRTY)
		return 0;
	if (vma->vm_file != file)
		return 0;
	if (vma->vm_ops && vma->vm_ops->close)
		return 0;
	if (!is_mergeable_vm_userfaultfd_ctx(vma, vm_userfaultfd_ctx))
		return 0;
	return 1;
}