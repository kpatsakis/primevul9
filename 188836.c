static void i40e_handle_link_event(struct i40e_pf *pf,
				   struct i40e_arq_event_info *e)
{
	struct i40e_aqc_get_link_status *status =
		(struct i40e_aqc_get_link_status *)&e->desc.params.raw;

	/* Do a new status request to re-enable LSE reporting
	 * and load new status information into the hw struct
	 * This completely ignores any state information
	 * in the ARQ event info, instead choosing to always
	 * issue the AQ update link status command.
	 */
	i40e_link_event(pf);

	/* Check if module meets thermal requirements */
	if (status->phy_type == I40E_PHY_TYPE_NOT_SUPPORTED_HIGH_TEMP) {
		dev_err(&pf->pdev->dev,
			"Rx/Tx is disabled on this device because the module does not meet thermal requirements.\n");
		dev_err(&pf->pdev->dev,
			"Refer to the Intel(R) Ethernet Adapters and Devices User Guide for a list of supported modules.\n");
	} else {
		/* check for unqualified module, if link is down, suppress
		 * the message if link was forced to be down.
		 */
		if ((status->link_info & I40E_AQ_MEDIA_AVAILABLE) &&
		    (!(status->an_info & I40E_AQ_QUALIFIED_MODULE)) &&
		    (!(status->link_info & I40E_AQ_LINK_UP)) &&
		    (!(pf->flags & I40E_FLAG_LINK_DOWN_ON_CLOSE_ENABLED))) {
			dev_err(&pf->pdev->dev,
				"Rx/Tx is disabled on this device because an unsupported SFP module type was detected.\n");
			dev_err(&pf->pdev->dev,
				"Refer to the Intel(R) Ethernet Adapters and Devices User Guide for a list of supported modules.\n");
		}
	}
}