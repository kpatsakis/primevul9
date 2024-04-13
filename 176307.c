static bool check_dax_vmas(struct vm_area_struct **vmas, long nr_pages)
{
	long i;
	struct vm_area_struct *vma_prev = NULL;

	for (i = 0; i < nr_pages; i++) {
		struct vm_area_struct *vma = vmas[i];

		if (vma == vma_prev)
			continue;

		vma_prev = vma;

		if (vma_is_fsdax(vma))
			return true;
	}
	return false;
}