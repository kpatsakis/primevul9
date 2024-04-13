static void __set_nx_huge_pages(bool val)
{
	nx_huge_pages = itlb_multihit_kvm_mitigation = val;
}