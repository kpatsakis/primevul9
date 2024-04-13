static struct vm_area_struct *remove_vma(struct vm_area_struct *vma)
{
	struct vm_area_struct *next = vma->vm_next;

	might_sleep();
	if (vma->vm_ops && vma->vm_ops->close)
		vma->vm_ops->close(vma);
	if (vma->vm_file)
		fput(vma->vm_file);
	mpol_put(vma_policy(vma));
	kmem_cache_free(vm_area_cachep, vma);
	return next;
}