static int temac_dcr_setup(struct temac_local *lp, struct platform_device *op,
				struct device_node *np)
{
	unsigned int dcrs;

	/* setup the dcr address mapping if it's in the device tree */

	dcrs = dcr_resource_start(np, 0);
	if (dcrs != 0) {
		lp->sdma_dcrs = dcr_map(np, dcrs, dcr_resource_len(np, 0));
		lp->dma_in = temac_dma_dcr_in;
		lp->dma_out = temac_dma_dcr_out;
		dev_dbg(&op->dev, "DCR base: %x\n", dcrs);
		return 0;
	}
	/* no DCR in the device tree, indicate a failure */
	return -1;
}