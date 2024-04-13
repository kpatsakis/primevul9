static void i40e_sync_filters_subtask(struct i40e_pf *pf)
{
	int v;

	if (!pf)
		return;
	if (!test_and_clear_bit(__I40E_MACVLAN_SYNC_PENDING, pf->state))
		return;
	if (test_and_set_bit(__I40E_VF_DISABLE, pf->state)) {
		set_bit(__I40E_MACVLAN_SYNC_PENDING, pf->state);
		return;
	}

	for (v = 0; v < pf->num_alloc_vsi; v++) {
		if (pf->vsi[v] &&
		    (pf->vsi[v]->flags & I40E_VSI_FLAG_FILTER_CHANGED)) {
			int ret = i40e_sync_vsi_filters(pf->vsi[v]);

			if (ret) {
				/* come back and try again later */
				set_bit(__I40E_MACVLAN_SYNC_PENDING,
					pf->state);
				break;
			}
		}
	}
	clear_bit(__I40E_VF_DISABLE, pf->state);
}