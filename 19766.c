static void sev_flush_guest_memory(struct vcpu_svm *svm, void *va,
				   unsigned long len)
{
	/*
	 * If hardware enforced cache coherency for encrypted mappings of the
	 * same physical page is supported, nothing to do.
	 */
	if (boot_cpu_has(X86_FEATURE_SME_COHERENT))
		return;

	/*
	 * If the VM Page Flush MSR is supported, use it to flush the page
	 * (using the page virtual address and the guest ASID).
	 */
	if (boot_cpu_has(X86_FEATURE_VM_PAGE_FLUSH)) {
		struct kvm_sev_info *sev;
		unsigned long va_start;
		u64 start, stop;

		/* Align start and stop to page boundaries. */
		va_start = (unsigned long)va;
		start = (u64)va_start & PAGE_MASK;
		stop = PAGE_ALIGN((u64)va_start + len);

		if (start < stop) {
			sev = &to_kvm_svm(svm->vcpu.kvm)->sev_info;

			while (start < stop) {
				wrmsrl(MSR_AMD64_VM_PAGE_FLUSH,
				       start | sev->asid);

				start += PAGE_SIZE;
			}

			return;
		}

		WARN(1, "Address overflow, using WBINVD\n");
	}

	/*
	 * Hardware should always have one of the above features,
	 * but if not, use WBINVD and issue a warning.
	 */
	WARN_ONCE(1, "Using WBINVD to flush guest memory\n");
	wbinvd_on_all_cpus();
}