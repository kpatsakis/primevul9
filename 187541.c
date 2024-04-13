static enum sas_device_type to_dev_type(struct discover_resp *dr)
{
	/* This is detecting a failure to transmit initial dev to host
	 * FIS as described in section J.5 of sas-2 r16
	 */
	if (dr->attached_dev_type == SAS_PHY_UNUSED && dr->attached_sata_dev &&
	    dr->linkrate >= SAS_LINK_RATE_1_5_GBPS)
		return SAS_SATA_PENDING;
	else
		return dr->attached_dev_type;
}