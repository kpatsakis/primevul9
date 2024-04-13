static bool kvm_has_zapped_obsolete_pages(struct kvm *kvm)
{
	return unlikely(!list_empty_careful(&kvm->arch.zapped_obsolete_pages));
}