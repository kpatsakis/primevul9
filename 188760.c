u32 i40e_get_current_atr_cnt(struct i40e_pf *pf)
{
	return i40e_get_current_fd_count(pf) - pf->fdir_pf_active_filters;
}