static int i40e_max_xdp_frame_size(struct i40e_vsi *vsi)
{
	if (PAGE_SIZE >= 8192 || (vsi->back->flags & I40E_FLAG_LEGACY_RX))
		return I40E_RXBUFFER_2048;
	else
		return I40E_RXBUFFER_3072;
}