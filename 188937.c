static void i40e_vsi_delete(struct i40e_vsi *vsi)
{
	/* remove default VSI is not allowed */
	if (vsi == vsi->back->vsi[vsi->back->lan_vsi])
		return;

	i40e_aq_delete_element(&vsi->back->hw, vsi->seid, NULL);
}