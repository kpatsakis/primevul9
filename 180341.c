find_vma_prev(struct mm_struct *mm, unsigned long addr,
			struct vm_area_struct **pprev)
{
	struct vm_area_struct *vma;

	vma = find_vma(mm, addr);
	if (vma) {
		*pprev = vma->vm_prev;
	} else {
		struct rb_node *rb_node = mm->mm_rb.rb_node;
		*pprev = NULL;
		while (rb_node) {
			*pprev = rb_entry(rb_node, struct vm_area_struct, vm_rb);
			rb_node = rb_node->rb_right;
		}
	}
	return vma;
}