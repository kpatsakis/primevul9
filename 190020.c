static void __clear_sp_write_flooding_count(struct kvm_mmu_page *sp)
{
	atomic_set(&sp->write_flooding_count,  0);
}