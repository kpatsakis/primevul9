static void svm_handle_mce(struct vcpu_svm *svm)
{
	if (is_erratum_383()) {
		/*
		 * Erratum 383 triggered. Guest state is corrupt so kill the
		 * guest.
		 */
		pr_err("KVM: Guest triggered AMD Erratum 383\n");

		kvm_make_request(KVM_REQ_TRIPLE_FAULT, &svm->vcpu);

		return;
	}

	/*
	 * On an #MC intercept the MCE handler is not called automatically in
	 * the host. So do it by hand here.
	 */
	kvm_machine_check();
}