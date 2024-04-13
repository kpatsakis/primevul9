__kvmgt_protect_table_find(struct kvmgt_guest_info *info, gfn_t gfn)
{
	struct kvmgt_pgfn *p, *res = NULL;

	hash_for_each_possible(info->ptable, p, hnode, gfn) {
		if (gfn == p->gfn) {
			res = p;
			break;
		}
	}

	return res;
}