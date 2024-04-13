
static int ql_poll(struct napi_struct *napi, int budget)
{
	struct ql3_adapter *qdev = container_of(napi, struct ql3_adapter, napi);
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	int work_done;

	work_done = ql_tx_rx_clean(qdev, budget);

	if (work_done < budget && napi_complete_done(napi, work_done)) {
		unsigned long flags;

		spin_lock_irqsave(&qdev->hw_lock, flags);
		ql_update_small_bufq_prod_index(qdev);
		ql_update_lrg_bufq_prod_index(qdev);
		writel(qdev->rsp_consumer_index,
			    &port_regs->CommonRegs.rspQConsumerIndex);
		spin_unlock_irqrestore(&qdev->hw_lock, flags);

		ql_enable_interrupts(qdev);
	}
	return work_done;