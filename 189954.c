static unsigned kvm_page_table_hashfn(gfn_t gfn)
{
	return hash_64(gfn, KVM_MMU_HASH_SHIFT);
}