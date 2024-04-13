const char *arch_vma_name(struct vm_area_struct *vma)
{
	if (vma->vm_flags & VM_MPX)
		return "[mpx]";
	return NULL;
}