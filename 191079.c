void svm_set_efer(struct kvm_vcpu *vcpu, u64 efer)
{
	vcpu->arch.efer = efer;

	if (!npt_enabled) {
		/* Shadow paging assumes NX to be available.  */
		efer |= EFER_NX;

		if (!(efer & EFER_LMA))
			efer &= ~EFER_LME;
	}

	to_svm(vcpu)->vmcb->save.efer = efer | EFER_SVME;
	mark_dirty(to_svm(vcpu)->vmcb, VMCB_CR);
}