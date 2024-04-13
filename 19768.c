static int __sev_launch_update_vmsa(struct kvm *kvm, struct kvm_vcpu *vcpu,
				    int *error)
{
	struct sev_data_launch_update_vmsa vmsa;
	struct vcpu_svm *svm = to_svm(vcpu);
	int ret;

	/* Perform some pre-encryption checks against the VMSA */
	ret = sev_es_sync_vmsa(svm);
	if (ret)
		return ret;

	/*
	 * The LAUNCH_UPDATE_VMSA command will perform in-place encryption of
	 * the VMSA memory content (i.e it will write the same memory region
	 * with the guest's key), so invalidate it first.
	 */
	clflush_cache_range(svm->vmsa, PAGE_SIZE);

	vmsa.reserved = 0;
	vmsa.handle = to_kvm_svm(kvm)->sev_info.handle;
	vmsa.address = __sme_pa(svm->vmsa);
	vmsa.len = PAGE_SIZE;
	return sev_issue_cmd(kvm, SEV_CMD_LAUNCH_UPDATE_VMSA, &vmsa, error);
}