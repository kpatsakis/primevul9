static int i40e_vsi_mem_alloc(struct i40e_pf *pf, enum i40e_vsi_type type)
{
	int ret = -ENODEV;
	struct i40e_vsi *vsi;
	int vsi_idx;
	int i;

	/* Need to protect the allocation of the VSIs at the PF level */
	mutex_lock(&pf->switch_mutex);

	/* VSI list may be fragmented if VSI creation/destruction has
	 * been happening.  We can afford to do a quick scan to look
	 * for any free VSIs in the list.
	 *
	 * find next empty vsi slot, looping back around if necessary
	 */
	i = pf->next_vsi;
	while (i < pf->num_alloc_vsi && pf->vsi[i])
		i++;
	if (i >= pf->num_alloc_vsi) {
		i = 0;
		while (i < pf->next_vsi && pf->vsi[i])
			i++;
	}

	if (i < pf->num_alloc_vsi && !pf->vsi[i]) {
		vsi_idx = i;             /* Found one! */
	} else {
		ret = -ENODEV;
		goto unlock_pf;  /* out of VSI slots! */
	}
	pf->next_vsi = ++i;

	vsi = kzalloc(sizeof(*vsi), GFP_KERNEL);
	if (!vsi) {
		ret = -ENOMEM;
		goto unlock_pf;
	}
	vsi->type = type;
	vsi->back = pf;
	set_bit(__I40E_VSI_DOWN, vsi->state);
	vsi->flags = 0;
	vsi->idx = vsi_idx;
	vsi->int_rate_limit = 0;
	vsi->rss_table_size = (vsi->type == I40E_VSI_MAIN) ?
				pf->rss_table_size : 64;
	vsi->netdev_registered = false;
	vsi->work_limit = I40E_DEFAULT_IRQ_WORK;
	hash_init(vsi->mac_filter_hash);
	vsi->irqs_ready = false;

	if (type == I40E_VSI_MAIN) {
		vsi->af_xdp_zc_qps = bitmap_zalloc(pf->num_lan_qps, GFP_KERNEL);
		if (!vsi->af_xdp_zc_qps)
			goto err_rings;
	}

	ret = i40e_set_num_rings_in_vsi(vsi);
	if (ret)
		goto err_rings;

	ret = i40e_vsi_alloc_arrays(vsi, true);
	if (ret)
		goto err_rings;

	/* Setup default MSIX irq handler for VSI */
	i40e_vsi_setup_irqhandler(vsi, i40e_msix_clean_rings);

	/* Initialize VSI lock */
	spin_lock_init(&vsi->mac_filter_hash_lock);
	pf->vsi[vsi_idx] = vsi;
	ret = vsi_idx;
	goto unlock_pf;

err_rings:
	bitmap_free(vsi->af_xdp_zc_qps);
	pf->next_vsi = i - 1;
	kfree(vsi);
unlock_pf:
	mutex_unlock(&pf->switch_mutex);
	return ret;
}