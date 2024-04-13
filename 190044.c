static struct kvm_mmu_page *kvm_mmu_alloc_page(struct kvm_vcpu *vcpu, int direct)
{
	struct kvm_mmu_page *sp;

	sp = kvm_mmu_memory_cache_alloc(&vcpu->arch.mmu_page_header_cache);
	sp->spt = kvm_mmu_memory_cache_alloc(&vcpu->arch.mmu_shadow_page_cache);
	if (!direct)
		sp->gfns = kvm_mmu_memory_cache_alloc(&vcpu->arch.mmu_gfn_array_cache);
	set_page_private(virt_to_page(sp->spt), (unsigned long)sp);

	/*
	 * active_mmu_pages must be a FIFO list, as kvm_zap_obsolete_pages()
	 * depends on valid pages being added to the head of the list.  See
	 * comments in kvm_zap_obsolete_pages().
	 */
	sp->mmu_valid_gen = vcpu->kvm->arch.mmu_valid_gen;
	list_add(&sp->link, &vcpu->kvm->arch.active_mmu_pages);
	kvm_mod_used_mmu_pages(vcpu->kvm, +1);
	return sp;
}