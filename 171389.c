 */
static void ql_update_small_bufq_prod_index(struct ql3_adapter *qdev)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;

	if (qdev->small_buf_release_cnt >= 16) {
		while (qdev->small_buf_release_cnt >= 16) {
			qdev->small_buf_q_producer_index++;

			if (qdev->small_buf_q_producer_index ==
			    NUM_SBUFQ_ENTRIES)
				qdev->small_buf_q_producer_index = 0;
			qdev->small_buf_release_cnt -= 8;
		}
		wmb();
		writel_relaxed(qdev->small_buf_q_producer_index,
			       &port_regs->CommonRegs.rxSmallQProducerIndex);
	}