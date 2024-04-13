static void i40e_veb_clear(struct i40e_veb *veb)
{
	if (!veb)
		return;

	if (veb->pf) {
		struct i40e_pf *pf = veb->pf;

		mutex_lock(&pf->switch_mutex);
		if (pf->veb[veb->idx] == veb)
			pf->veb[veb->idx] = NULL;
		mutex_unlock(&pf->switch_mutex);
	}

	kfree(veb);
}