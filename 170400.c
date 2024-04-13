static void bpf_link_free_id(int id)
{
	if (!id)
		return;

	spin_lock_bh(&link_idr_lock);
	idr_remove(&link_idr, id);
	spin_unlock_bh(&link_idr_lock);
}