static void htab_elem_free_rcu(struct rcu_head *head)
{
	struct htab_elem *l = container_of(head, struct htab_elem, rcu);
	struct bpf_htab *htab = l->htab;

	htab_elem_free(htab, l);
}