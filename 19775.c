void sev_vcpu_deliver_sipi_vector(struct kvm_vcpu *vcpu, u8 vector)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	/* First SIPI: Use the values as initially set by the VMM */
	if (!svm->received_first_sipi) {
		svm->received_first_sipi = true;
		return;
	}

	/*
	 * Subsequent SIPI: Return from an AP Reset Hold VMGEXIT, where
	 * the guest will set the CS and RIP. Set SW_EXIT_INFO_2 to a
	 * non-zero value.
	 */
	if (!svm->ghcb)
		return;

	ghcb_set_sw_exit_info_2(svm->ghcb, 1);
}