void kvm_mmu_x86_module_init(void)
{
	if (nx_huge_pages == -1)
		__set_nx_huge_pages(get_nx_auto_mode());
}