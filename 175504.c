static struct htab_elem *get_htab_elem(struct bpf_htab *htab, int i)
{
	return (struct htab_elem *) (htab->elems + i * (u64)htab->elem_size);
}