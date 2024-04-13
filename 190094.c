static void shadow_walk_init(struct kvm_shadow_walk_iterator *iterator,
			     struct kvm_vcpu *vcpu, u64 addr)
{
	shadow_walk_init_using_root(iterator, vcpu, vcpu->arch.mmu->root.hpa,
				    addr);
}