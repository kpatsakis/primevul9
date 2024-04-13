static unsigned long kvmgt_gfn_to_pfn(unsigned long handle, unsigned long gfn)
{
	struct kvmgt_guest_info *info;
	kvm_pfn_t pfn;

	if (!handle_valid(handle))
		return INTEL_GVT_INVALID_ADDR;

	info = (struct kvmgt_guest_info *)handle;

	pfn = gfn_to_pfn(info->kvm, gfn);
	if (is_error_noslot_pfn(pfn))
		return INTEL_GVT_INVALID_ADDR;

	return pfn;
}