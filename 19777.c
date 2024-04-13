void sev_es_prepare_guest_switch(struct vcpu_svm *svm, unsigned int cpu)
{
	struct svm_cpu_data *sd = per_cpu(svm_data, cpu);
	struct vmcb_save_area *hostsa;

	/*
	 * As an SEV-ES guest, hardware will restore the host state on VMEXIT,
	 * of which one step is to perform a VMLOAD. Since hardware does not
	 * perform a VMSAVE on VMRUN, the host savearea must be updated.
	 */
	vmsave(__sme_page_pa(sd->save_area));

	/* XCR0 is restored on VMEXIT, save the current host value */
	hostsa = (struct vmcb_save_area *)(page_address(sd->save_area) + 0x400);
	hostsa->xcr0 = xgetbv(XCR_XFEATURE_ENABLED_MASK);

	/* PKRU is restored on VMEXIT, save the current host value */
	hostsa->pkru = read_pkru();

	/* MSR_IA32_XSS is restored on VMEXIT, save the currnet host value */
	hostsa->xss = host_xss;
}