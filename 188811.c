void __i40e_del_filter(struct i40e_vsi *vsi, struct i40e_mac_filter *f)
{
	if (!f)
		return;

	/* If the filter was never added to firmware then we can just delete it
	 * directly and we don't want to set the status to remove or else an
	 * admin queue command will unnecessarily fire.
	 */
	if ((f->state == I40E_FILTER_FAILED) ||
	    (f->state == I40E_FILTER_NEW)) {
		hash_del(&f->hlist);
		kfree(f);
	} else {
		f->state = I40E_FILTER_REMOVE;
	}

	vsi->flags |= I40E_VSI_FLAG_FILTER_CHANGED;
	set_bit(__I40E_MACVLAN_SYNC_PENDING, vsi->back->state);
}