void i40e_set_fec_in_flags(u8 fec_cfg, u32 *flags)
{
	if (fec_cfg & I40E_AQ_SET_FEC_AUTO)
		*flags |= I40E_FLAG_RS_FEC | I40E_FLAG_BASE_R_FEC;
	if ((fec_cfg & I40E_AQ_SET_FEC_REQUEST_RS) ||
	    (fec_cfg & I40E_AQ_SET_FEC_ABILITY_RS)) {
		*flags |= I40E_FLAG_RS_FEC;
		*flags &= ~I40E_FLAG_BASE_R_FEC;
	}
	if ((fec_cfg & I40E_AQ_SET_FEC_REQUEST_KR) ||
	    (fec_cfg & I40E_AQ_SET_FEC_ABILITY_KR)) {
		*flags |= I40E_FLAG_BASE_R_FEC;
		*flags &= ~I40E_FLAG_RS_FEC;
	}
	if (fec_cfg == 0)
		*flags &= ~(I40E_FLAG_RS_FEC | I40E_FLAG_BASE_R_FEC);
}