void i40e_do_reset(struct i40e_pf *pf, u32 reset_flags, bool lock_acquired)
{
	u32 val;

	WARN_ON(in_interrupt());


	/* do the biggest reset indicated */
	if (reset_flags & BIT_ULL(__I40E_GLOBAL_RESET_REQUESTED)) {

		/* Request a Global Reset
		 *
		 * This will start the chip's countdown to the actual full
		 * chip reset event, and a warning interrupt to be sent
		 * to all PFs, including the requestor.  Our handler
		 * for the warning interrupt will deal with the shutdown
		 * and recovery of the switch setup.
		 */
		dev_dbg(&pf->pdev->dev, "GlobalR requested\n");
		val = rd32(&pf->hw, I40E_GLGEN_RTRIG);
		val |= I40E_GLGEN_RTRIG_GLOBR_MASK;
		wr32(&pf->hw, I40E_GLGEN_RTRIG, val);

	} else if (reset_flags & BIT_ULL(__I40E_CORE_RESET_REQUESTED)) {

		/* Request a Core Reset
		 *
		 * Same as Global Reset, except does *not* include the MAC/PHY
		 */
		dev_dbg(&pf->pdev->dev, "CoreR requested\n");
		val = rd32(&pf->hw, I40E_GLGEN_RTRIG);
		val |= I40E_GLGEN_RTRIG_CORER_MASK;
		wr32(&pf->hw, I40E_GLGEN_RTRIG, val);
		i40e_flush(&pf->hw);

	} else if (reset_flags & I40E_PF_RESET_FLAG) {

		/* Request a PF Reset
		 *
		 * Resets only the PF-specific registers
		 *
		 * This goes directly to the tear-down and rebuild of
		 * the switch, since we need to do all the recovery as
		 * for the Core Reset.
		 */
		dev_dbg(&pf->pdev->dev, "PFR requested\n");
		i40e_handle_reset_warning(pf, lock_acquired);

		dev_info(&pf->pdev->dev,
			 pf->flags & I40E_FLAG_DISABLE_FW_LLDP ?
			 "FW LLDP is disabled\n" :
			 "FW LLDP is enabled\n");

	} else if (reset_flags & BIT_ULL(__I40E_REINIT_REQUESTED)) {
		int v;

		/* Find the VSI(s) that requested a re-init */
		dev_info(&pf->pdev->dev,
			 "VSI reinit requested\n");
		for (v = 0; v < pf->num_alloc_vsi; v++) {
			struct i40e_vsi *vsi = pf->vsi[v];

			if (vsi != NULL &&
			    test_and_clear_bit(__I40E_VSI_REINIT_REQUESTED,
					       vsi->state))
				i40e_vsi_reinit_locked(pf->vsi[v]);
		}
	} else if (reset_flags & BIT_ULL(__I40E_DOWN_REQUESTED)) {
		int v;

		/* Find the VSI(s) that needs to be brought down */
		dev_info(&pf->pdev->dev, "VSI down requested\n");
		for (v = 0; v < pf->num_alloc_vsi; v++) {
			struct i40e_vsi *vsi = pf->vsi[v];

			if (vsi != NULL &&
			    test_and_clear_bit(__I40E_VSI_DOWN_REQUESTED,
					       vsi->state)) {
				set_bit(__I40E_VSI_DOWN, vsi->state);
				i40e_down(vsi);
			}
		}
	} else {
		dev_info(&pf->pdev->dev,
			 "bad reset request 0x%08x\n", reset_flags);
	}
}