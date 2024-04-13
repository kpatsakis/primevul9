struct vm_area_struct *copy_vma(struct vm_area_struct **vmap,
	unsigned long addr, unsigned long len, pgoff_t pgoff,
	bool *need_rmap_locks)
{
	struct vm_area_struct *vma = *vmap;
	unsigned long vma_start = vma->vm_start;
	struct mm_struct *mm = vma->vm_mm;
	struct vm_area_struct *new_vma, *prev;
	struct rb_node **rb_link, *rb_parent;
	bool faulted_in_anon_vma = true;

	/*
	 * If anonymous vma has not yet been faulted, update new pgoff
	 * to match new location, to increase its chance of merging.
	 */
	if (unlikely(vma_is_anonymous(vma) && !vma->anon_vma)) {
		pgoff = addr >> PAGE_SHIFT;
		faulted_in_anon_vma = false;
	}

	if (find_vma_links(mm, addr, addr + len, &prev, &rb_link, &rb_parent))
		return NULL;	/* should never get here */
	new_vma = vma_merge(mm, prev, addr, addr + len, vma->vm_flags,
			    vma->anon_vma, vma->vm_file, pgoff, vma_policy(vma),
			    vma->vm_userfaultfd_ctx);
	if (new_vma) {
		/*
		 * Source vma may have been merged into new_vma
		 */
		if (unlikely(vma_start >= new_vma->vm_start &&
			     vma_start < new_vma->vm_end)) {
			/*
			 * The only way we can get a vma_merge with
			 * self during an mremap is if the vma hasn't
			 * been faulted in yet and we were allowed to
			 * reset the dst vma->vm_pgoff to the
			 * destination address of the mremap to allow
			 * the merge to happen. mremap must change the
			 * vm_pgoff linearity between src and dst vmas
			 * (in turn preventing a vma_merge) to be
			 * safe. It is only safe to keep the vm_pgoff
			 * linear if there are no pages mapped yet.
			 */
			VM_BUG_ON_VMA(faulted_in_anon_vma, new_vma);
			*vmap = vma = new_vma;
		}
		*need_rmap_locks = (new_vma->vm_pgoff <= vma->vm_pgoff);
	} else {
		new_vma = kmem_cache_alloc(vm_area_cachep, GFP_KERNEL);
		if (!new_vma)
			goto out;
		*new_vma = *vma;
		new_vma->vm_start = addr;
		new_vma->vm_end = addr + len;
		new_vma->vm_pgoff = pgoff;
		if (vma_dup_policy(vma, new_vma))
			goto out_free_vma;
		INIT_LIST_HEAD(&new_vma->anon_vma_chain);
		if (anon_vma_clone(new_vma, vma))
			goto out_free_mempol;
		if (new_vma->vm_file)
			get_file(new_vma->vm_file);
		if (new_vma->vm_ops && new_vma->vm_ops->open)
			new_vma->vm_ops->open(new_vma);
		vma_link(mm, new_vma, prev, rb_link, rb_parent);
		*need_rmap_locks = false;
	}
	return new_vma;

out_free_mempol:
	mpol_put(vma_policy(new_vma));
out_free_vma:
	kmem_cache_free(vm_area_cachep, new_vma);
out:
	return NULL;
}