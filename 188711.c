static int i40e_pf_wait_queues_disabled(struct i40e_pf *pf)
{
	int v, ret = 0;

	for (v = 0; v < pf->hw.func_caps.num_vsis; v++) {
		if (pf->vsi[v]) {
			ret = i40e_vsi_wait_queues_disabled(pf->vsi[v]);
			if (ret)
				break;
		}
	}

	return ret;
}