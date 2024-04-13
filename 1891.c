int svm_allocate_nested(struct vcpu_svm *svm)
{
	struct page *vmcb02_page;

	if (svm->nested.initialized)
		return 0;

	vmcb02_page = alloc_page(GFP_KERNEL_ACCOUNT | __GFP_ZERO);
	if (!vmcb02_page)
		return -ENOMEM;
	svm->nested.vmcb02.ptr = page_address(vmcb02_page);
	svm->nested.vmcb02.pa = __sme_set(page_to_pfn(vmcb02_page) << PAGE_SHIFT);

	svm->nested.msrpm = svm_vcpu_alloc_msrpm();
	if (!svm->nested.msrpm)
		goto err_free_vmcb02;
	svm_vcpu_init_msrpm(&svm->vcpu, svm->nested.msrpm);

	svm->nested.initialized = true;
	return 0;

err_free_vmcb02:
	__free_page(vmcb02_page);
	return -ENOMEM;
}