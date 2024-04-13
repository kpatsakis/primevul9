static void vma_gap_update(struct vm_area_struct *vma)
{
	/*
	 * As it turns out, RB_DECLARE_CALLBACKS() already created a callback
	 * function that does exacltly what we want.
	 */
	vma_gap_callbacks_propagate(&vma->vm_rb, NULL);
}