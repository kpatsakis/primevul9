void bpf_link_inc(struct bpf_link *link)
{
	atomic64_inc(&link->refcnt);
}