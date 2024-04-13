static void i40e_vsi_update_queue_map(struct i40e_vsi *vsi,
				      struct i40e_vsi_context *ctxt)
{
	/* copy just the sections touched not the entire info
	 * since not all sections are valid as returned by
	 * update vsi params
	 */
	vsi->info.mapping_flags = ctxt->info.mapping_flags;
	memcpy(&vsi->info.queue_mapping,
	       &ctxt->info.queue_mapping, sizeof(vsi->info.queue_mapping));
	memcpy(&vsi->info.tc_mapping, ctxt->info.tc_mapping,
	       sizeof(vsi->info.tc_mapping));
}