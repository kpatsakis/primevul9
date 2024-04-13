static hpa_t mmu_alloc_root(struct kvm_vcpu *vcpu, gfn_t gfn, gva_t gva,
			    u8 level, bool direct)
{
	struct kvm_mmu_page *sp;

	sp = kvm_mmu_get_page(vcpu, gfn, gva, level, direct, ACC_ALL);
	++sp->root_count;

	return __pa(sp->spt);
}