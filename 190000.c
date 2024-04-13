void kvm_mmu_gfn_disallow_lpage(const struct kvm_memory_slot *slot, gfn_t gfn)
{
	update_gfn_disallow_lpage_count(slot, gfn, 1);
}