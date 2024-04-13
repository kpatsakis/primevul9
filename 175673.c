static void kvmgt_protect_table_del(struct kvmgt_guest_info *info,
				gfn_t gfn)
{
	struct kvmgt_pgfn *p;

	p = __kvmgt_protect_table_find(info, gfn);
	if (p) {
		hash_del(&p->hnode);
		kfree(p);
	}
}