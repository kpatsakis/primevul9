static int bnx2x_do_flr(struct bnx2x *bp)
{
	struct pci_dev *dev = bp->pdev;

	if (CHIP_IS_E1x(bp)) {
		BNX2X_DEV_INFO("FLR not supported in E1/E1H\n");
		return -EINVAL;
	}

	/* only bootcode REQ_BC_VER_4_INITIATE_FLR and onwards support flr */
	if (bp->common.bc_ver < REQ_BC_VER_4_INITIATE_FLR) {
		BNX2X_ERR("FLR not supported by BC_VER: 0x%x\n",
			  bp->common.bc_ver);
		return -EINVAL;
	}

	if (!pci_wait_for_pending_transaction(dev))
		dev_err(&dev->dev, "transaction is not cleared; proceeding with reset anyway\n");

	BNX2X_DEV_INFO("Initiating FLR\n");
	bnx2x_fw_command(bp, DRV_MSG_CODE_INITIATE_FLR, 0);

	return 0;
}