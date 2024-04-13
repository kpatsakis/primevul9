static void i40e_send_version(struct i40e_pf *pf)
{
	struct i40e_driver_version dv;

	dv.major_version = DRV_VERSION_MAJOR;
	dv.minor_version = DRV_VERSION_MINOR;
	dv.build_version = DRV_VERSION_BUILD;
	dv.subbuild_version = 0;
	strlcpy(dv.driver_string, DRV_VERSION, sizeof(dv.driver_string));
	i40e_aq_send_driver_version(&pf->hw, &dv, NULL);
}