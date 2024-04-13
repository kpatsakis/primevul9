static struct nfs4_slot *nfs4_alloc_slots(u32 max_slots, gfp_t gfp_flags)
{
	return kcalloc(max_slots, sizeof(struct nfs4_slot), gfp_flags);
}