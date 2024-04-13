static bool is_access_allowed(struct kvm_page_fault *fault, u64 spte)
{
	if (fault->exec)
		return is_executable_pte(spte);

	if (fault->write)
		return is_writable_pte(spte);

	/* Fault was on Read access */
	return spte & PT_PRESENT_MASK;
}