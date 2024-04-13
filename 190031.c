static void kvm_unsync_page(struct kvm *kvm, struct kvm_mmu_page *sp)
{
	trace_kvm_mmu_unsync_page(sp);
	++kvm->stat.mmu_unsync;
	sp->unsync = 1;

	kvm_mmu_mark_parents_unsync(sp);
}