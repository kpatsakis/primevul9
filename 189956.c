static bool page_fault_handle_page_track(struct kvm_vcpu *vcpu,
					 struct kvm_page_fault *fault)
{
	if (unlikely(fault->rsvd))
		return false;

	if (!fault->present || !fault->write)
		return false;

	/*
	 * guest is writing the page which is write tracked which can
	 * not be fixed by page fault handler.
	 */
	if (kvm_slot_page_track_is_active(vcpu->kvm, fault->slot, fault->gfn, KVM_PAGE_TRACK_WRITE))
		return true;

	return false;
}