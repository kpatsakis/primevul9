struct bpf_link *bpf_link_by_id(u32 id)
{
	struct bpf_link *link;

	if (!id)
		return ERR_PTR(-ENOENT);

	spin_lock_bh(&link_idr_lock);
	/* before link is "settled", ID is 0, pretend it doesn't exist yet */
	link = idr_find(&link_idr, id);
	if (link) {
		if (link->id)
			link = bpf_link_inc_not_zero(link);
		else
			link = ERR_PTR(-EAGAIN);
	} else {
		link = ERR_PTR(-ENOENT);
	}
	spin_unlock_bh(&link_idr_lock);
	return link;
}