void bnx2x_setup_cnic_irq_info(struct bnx2x *bp)
{
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;

	if (bp->flags & USING_MSIX_FLAG) {
		cp->drv_state |= CNIC_DRV_STATE_USING_MSIX;
		cp->irq_arr[0].irq_flags |= CNIC_IRQ_FL_MSIX;
		cp->irq_arr[0].vector = bp->msix_table[1].vector;
	} else {
		cp->drv_state &= ~CNIC_DRV_STATE_USING_MSIX;
		cp->irq_arr[0].irq_flags &= ~CNIC_IRQ_FL_MSIX;
	}
	if (!CHIP_IS_E1x(bp))
		cp->irq_arr[0].status_blk = (void *)bp->cnic_sb.e2_sb;
	else
		cp->irq_arr[0].status_blk = (void *)bp->cnic_sb.e1x_sb;

	cp->irq_arr[0].status_blk_num =  bnx2x_cnic_fw_sb_id(bp);
	cp->irq_arr[0].status_blk_num2 = bnx2x_cnic_igu_sb_id(bp);
	cp->irq_arr[1].status_blk = bp->def_status_blk;
	cp->irq_arr[1].status_blk_num = DEF_SB_ID;
	cp->irq_arr[1].status_blk_num2 = DEF_SB_IGU_ID;

	cp->num_irq = 2;
}