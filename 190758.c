static void destroy_pagetable_cache(struct mm_struct *mm)
{
	void *frag;

	frag = mm->context.pte_frag;
	if (frag)
		pte_frag_destroy(frag);

	frag = mm->context.pmd_frag;
	if (frag)
		pmd_frag_destroy(frag);
	return;
}