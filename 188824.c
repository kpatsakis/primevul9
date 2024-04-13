int i40e_up(struct i40e_vsi *vsi)
{
	int err;

	err = i40e_vsi_configure(vsi);
	if (!err)
		err = i40e_up_complete(vsi);

	return err;
}