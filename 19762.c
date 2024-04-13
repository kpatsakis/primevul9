void sev_es_create_vcpu(struct vcpu_svm *svm)
{
	/*
	 * Set the GHCB MSR value as per the GHCB specification when creating
	 * a vCPU for an SEV-ES guest.
	 */
	set_ghcb_msr(svm, GHCB_MSR_SEV_INFO(GHCB_VERSION_MAX,
					    GHCB_VERSION_MIN,
					    sev_enc_bit));
}