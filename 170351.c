static int bpf_link_alloc_id(struct bpf_link *link)
{
	int id;

	idr_preload(GFP_KERNEL);
	spin_lock_bh(&link_idr_lock);
	id = idr_alloc_cyclic(&link_idr, link, 1, INT_MAX, GFP_ATOMIC);
	spin_unlock_bh(&link_idr_lock);
	idr_preload_end();

	return id;
}