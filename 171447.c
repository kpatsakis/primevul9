
static irqreturn_t ql3xxx_isr(int irq, void *dev_id)
{

	struct net_device *ndev = dev_id;
	struct ql3_adapter *qdev = netdev_priv(ndev);
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	u32 value;
	int handled = 1;
	u32 var;

	value = ql_read_common_reg_l(qdev,
				     &port_regs->CommonRegs.ispControlStatus);

	if (value & (ISP_CONTROL_FE | ISP_CONTROL_RI)) {
		spin_lock(&qdev->adapter_lock);
		netif_stop_queue(qdev->ndev);
		netif_carrier_off(qdev->ndev);
		ql_disable_interrupts(qdev);
		qdev->port_link_state = LS_DOWN;
		set_bit(QL_RESET_ACTIVE, &qdev->flags) ;

		if (value & ISP_CONTROL_FE) {
			/*
			 * Chip Fatal Error.
			 */
			var =
			    ql_read_page0_reg_l(qdev,
					      &port_regs->PortFatalErrStatus);
			netdev_warn(ndev,
				    "Resetting chip. PortFatalErrStatus register = 0x%x\n",
				    var);
			set_bit(QL_RESET_START, &qdev->flags) ;
		} else {
			/*
			 * Soft Reset Requested.
			 */
			set_bit(QL_RESET_PER_SCSI, &qdev->flags) ;
			netdev_err(ndev,
				   "Another function issued a reset to the chip. ISR value = %x\n",
				   value);
		}
		queue_delayed_work(qdev->workqueue, &qdev->reset_work, 0);
		spin_unlock(&qdev->adapter_lock);
	} else if (value & ISP_IMR_DISABLE_CMPL_INT) {
		ql_disable_interrupts(qdev);
		if (likely(napi_schedule_prep(&qdev->napi)))
			__napi_schedule(&qdev->napi);
	} else
		return IRQ_NONE;

	return IRQ_RETVAL(handled);