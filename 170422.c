static struct bpf_link *bpf_link_inc_not_zero(struct bpf_link *link)
{
	return atomic64_fetch_add_unless(&link->refcnt, 1, 0) ? link : ERR_PTR(-ENOENT);
}