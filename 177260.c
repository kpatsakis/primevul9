static void sdma_wait_for_packet_egress(struct sdma_engine *sde,
					int pause)
{
	u64 off = 8 * sde->this_idx;
	struct hfi1_devdata *dd = sde->dd;
	int lcnt = 0;
	u64 reg_prev;
	u64 reg = 0;

	while (1) {
		reg_prev = reg;
		reg = read_csr(dd, off + SEND_EGRESS_SEND_DMA_STATUS);

		reg &= SDMA_EGRESS_PACKET_OCCUPANCY_SMASK;
		reg >>= SDMA_EGRESS_PACKET_OCCUPANCY_SHIFT;
		if (reg == 0)
			break;
		/* counter is reest if accupancy count changes */
		if (reg != reg_prev)
			lcnt = 0;
		if (lcnt++ > 500) {
			/* timed out - bounce the link */
			dd_dev_err(dd, "%s: engine %u timeout waiting for packets to egress, remaining count %u, bouncing link\n",
				   __func__, sde->this_idx, (u32)reg);
			queue_work(dd->pport->link_wq,
				   &dd->pport->link_bounce_work);
			break;
		}
		udelay(1);
	}
}