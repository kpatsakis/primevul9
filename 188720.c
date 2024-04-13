static int i40e_vsi_setup_vectors(struct i40e_vsi *vsi)
{
	int ret = -ENOENT;
	struct i40e_pf *pf = vsi->back;

	if (vsi->q_vectors[0]) {
		dev_info(&pf->pdev->dev, "VSI %d has existing q_vectors\n",
			 vsi->seid);
		return -EEXIST;
	}

	if (vsi->base_vector) {
		dev_info(&pf->pdev->dev, "VSI %d has non-zero base vector %d\n",
			 vsi->seid, vsi->base_vector);
		return -EEXIST;
	}

	ret = i40e_vsi_alloc_q_vectors(vsi);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "failed to allocate %d q_vector for VSI %d, ret=%d\n",
			 vsi->num_q_vectors, vsi->seid, ret);
		vsi->num_q_vectors = 0;
		goto vector_setup_out;
	}

	/* In Legacy mode, we do not have to get any other vector since we
	 * piggyback on the misc/ICR0 for queue interrupts.
	*/
	if (!(pf->flags & I40E_FLAG_MSIX_ENABLED))
		return ret;
	if (vsi->num_q_vectors)
		vsi->base_vector = i40e_get_lump(pf, pf->irq_pile,
						 vsi->num_q_vectors, vsi->idx);
	if (vsi->base_vector < 0) {
		dev_info(&pf->pdev->dev,
			 "failed to get tracking for %d vectors for VSI %d, err=%d\n",
			 vsi->num_q_vectors, vsi->seid, vsi->base_vector);
		i40e_vsi_free_q_vectors(vsi);
		ret = -ENOENT;
		goto vector_setup_out;
	}

vector_setup_out:
	return ret;
}