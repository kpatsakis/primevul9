static struct vm_area_struct *__install_special_mapping(
	struct mm_struct *mm,
	unsigned long addr, unsigned long len,
	unsigned long vm_flags, void *priv,
	const struct vm_operations_struct *ops)
{
	int ret;
	struct vm_area_struct *vma;

	vma = kmem_cache_zalloc(vm_area_cachep, GFP_KERNEL);
	if (unlikely(vma == NULL))
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(&vma->anon_vma_chain);
	vma->vm_mm = mm;
	vma->vm_start = addr;
	vma->vm_end = addr + len;

	vma->vm_flags = vm_flags | mm->def_flags | VM_DONTEXPAND | VM_SOFTDIRTY;
	vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);

	vma->vm_ops = ops;
	vma->vm_private_data = priv;

	ret = insert_vm_struct(mm, vma);
	if (ret)
		goto out;

	vm_stat_account(mm, vma->vm_flags, len >> PAGE_SHIFT);

	perf_event_mmap(vma);

	return vma;

out:
	kmem_cache_free(vm_area_cachep, vma);
	return ERR_PTR(ret);
}