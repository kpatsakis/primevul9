u32 i40e_get_cur_guaranteed_fd_count(struct i40e_pf *pf)
{
	u32 val, fcnt_prog;

	val = rd32(&pf->hw, I40E_PFQF_FDSTAT);
	fcnt_prog = (val & I40E_PFQF_FDSTAT_GUARANT_CNT_MASK);
	return fcnt_prog;
}