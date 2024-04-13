static void i40e_vsi_free_q_vectors(struct i40e_vsi *vsi)
{
	int v_idx;

	for (v_idx = 0; v_idx < vsi->num_q_vectors; v_idx++)
		i40e_free_q_vector(vsi, v_idx);
}