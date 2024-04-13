static void mark_unsync(u64 *spte)
{
	struct kvm_mmu_page *sp;
	unsigned int index;

	sp = sptep_to_sp(spte);
	index = spte - sp->spt;
	if (__test_and_set_bit(index, sp->unsync_child_bitmap))
		return;
	if (sp->unsync_children++)
		return;
	kvm_mmu_mark_parents_unsync(sp);
}