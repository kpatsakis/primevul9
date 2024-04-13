
static int ql_tx_rx_clean(struct ql3_adapter *qdev, int budget)
{
	struct net_rsp_iocb *net_rsp;
	struct net_device *ndev = qdev->ndev;
	int work_done = 0;

	/* While there are entries in the completion queue. */
	while ((le32_to_cpu(*(qdev->prsp_producer_index)) !=
		qdev->rsp_consumer_index) && (work_done < budget)) {

		net_rsp = qdev->rsp_current;
		rmb();
		/*
		 * Fix 4032 chip's undocumented "feature" where bit-8 is set
		 * if the inbound completion is for a VLAN.
		 */
		if (qdev->device_id == QL3032_DEVICE_ID)
			net_rsp->opcode &= 0x7f;
		switch (net_rsp->opcode) {

		case OPCODE_OB_MAC_IOCB_FN0:
		case OPCODE_OB_MAC_IOCB_FN2:
			ql_process_mac_tx_intr(qdev, (struct ob_mac_iocb_rsp *)
					       net_rsp);
			break;

		case OPCODE_IB_MAC_IOCB:
		case OPCODE_IB_3032_MAC_IOCB:
			ql_process_mac_rx_intr(qdev, (struct ib_mac_iocb_rsp *)
					       net_rsp);
			work_done++;
			break;

		case OPCODE_IB_IP_IOCB:
		case OPCODE_IB_3032_IP_IOCB:
			ql_process_macip_rx_intr(qdev, (struct ib_ip_iocb_rsp *)
						 net_rsp);
			work_done++;
			break;
		default: {
			u32 *tmp = (u32 *)net_rsp;
			netdev_err(ndev,
				   "Hit default case, not handled!\n"
				   "	dropping the packet, opcode = %x\n"
				   "0x%08lx 0x%08lx 0x%08lx 0x%08lx\n",
				   net_rsp->opcode,
				   (unsigned long int)tmp[0],
				   (unsigned long int)tmp[1],
				   (unsigned long int)tmp[2],
				   (unsigned long int)tmp[3]);
		}
		}

		qdev->rsp_consumer_index++;

		if (qdev->rsp_consumer_index == NUM_RSP_Q_ENTRIES) {
			qdev->rsp_consumer_index = 0;
			qdev->rsp_current = qdev->rsp_q_virt_addr;
		} else {
			qdev->rsp_current++;
		}

	}

	return work_done;