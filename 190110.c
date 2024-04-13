static void kvm_unlink_unsync_page(struct kvm *kvm, struct kvm_mmu_page *sp)
{
	WARN_ON(!sp->unsync);
	trace_kvm_mmu_sync_page(sp);
	sp->unsync = 0;
	--kvm->stat.mmu_unsync;
}