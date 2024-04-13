static int i40e_vsi_clear(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf;

	if (!vsi)
		return 0;

	if (!vsi->back)
		goto free_vsi;
	pf = vsi->back;

	mutex_lock(&pf->switch_mutex);
	if (!pf->vsi[vsi->idx]) {
		dev_err(&pf->pdev->dev, "pf->vsi[%d] is NULL, just free vsi[%d](type %d)\n",
			vsi->idx, vsi->idx, vsi->type);
		goto unlock_vsi;
	}

	if (pf->vsi[vsi->idx] != vsi) {
		dev_err(&pf->pdev->dev,
			"pf->vsi[%d](type %d) != vsi[%d](type %d): no free!\n",
			pf->vsi[vsi->idx]->idx,
			pf->vsi[vsi->idx]->type,
			vsi->idx, vsi->type);
		goto unlock_vsi;
	}

	/* updates the PF for this cleared vsi */
	i40e_put_lump(pf->qp_pile, vsi->base_queue, vsi->idx);
	i40e_put_lump(pf->irq_pile, vsi->base_vector, vsi->idx);

	bitmap_free(vsi->af_xdp_zc_qps);
	i40e_vsi_free_arrays(vsi, true);
	i40e_clear_rss_config_user(vsi);

	pf->vsi[vsi->idx] = NULL;
	if (vsi->idx < pf->next_vsi)
		pf->next_vsi = vsi->idx;

unlock_vsi:
	mutex_unlock(&pf->switch_mutex);
free_vsi:
	kfree(vsi);

	return 0;
}