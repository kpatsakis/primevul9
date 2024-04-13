static int i40e_enter_busy_conf(struct i40e_vsi *vsi)
{
	struct i40e_pf *pf = vsi->back;
	int timeout = 50;

	while (test_and_set_bit(__I40E_CONFIG_BUSY, pf->state)) {
		timeout--;
		if (!timeout)
			return -EBUSY;
		usleep_range(1000, 2000);
	}

	return 0;
}