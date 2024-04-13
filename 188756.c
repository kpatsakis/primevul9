u32 i40e_get_global_fd_count(struct i40e_pf *pf)
{
	u32 val, fcnt_prog;

	val = rd32(&pf->hw, I40E_GLQF_FDCNT_0);
	fcnt_prog = (val & I40E_GLQF_FDCNT_0_GUARANT_CNT_MASK) +
		    ((val & I40E_GLQF_FDCNT_0_BESTCNT_MASK) >>
		     I40E_GLQF_FDCNT_0_BESTCNT_SHIFT);
	return fcnt_prog;
}