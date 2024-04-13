 */
static void ql_update_lrg_bufq_prod_index(struct ql3_adapter *qdev)
{
	struct bufq_addr_element *lrg_buf_q_ele;
	int i;
	struct ql_rcv_buf_cb *lrg_buf_cb;
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;

	if ((qdev->lrg_buf_free_count >= 8) &&
	    (qdev->lrg_buf_release_cnt >= 16)) {

		if (qdev->lrg_buf_skb_check)
			if (!ql_populate_free_queue(qdev))
				return;

		lrg_buf_q_ele = qdev->lrg_buf_next_free;

		while ((qdev->lrg_buf_release_cnt >= 16) &&
		       (qdev->lrg_buf_free_count >= 8)) {

			for (i = 0; i < 8; i++) {
				lrg_buf_cb =
				    ql_get_from_lrg_buf_free_list(qdev);
				lrg_buf_q_ele->addr_high =
				    lrg_buf_cb->buf_phy_addr_high;
				lrg_buf_q_ele->addr_low =
				    lrg_buf_cb->buf_phy_addr_low;
				lrg_buf_q_ele++;

				qdev->lrg_buf_release_cnt--;
			}

			qdev->lrg_buf_q_producer_index++;

			if (qdev->lrg_buf_q_producer_index ==
			    qdev->num_lbufq_entries)
				qdev->lrg_buf_q_producer_index = 0;

			if (qdev->lrg_buf_q_producer_index ==
			    (qdev->num_lbufq_entries - 1)) {
				lrg_buf_q_ele = qdev->lrg_buf_q_virt_addr;
			}
		}
		wmb();
		qdev->lrg_buf_next_free = lrg_buf_q_ele;
		writel(qdev->lrg_buf_q_producer_index,
			&port_regs->CommonRegs.rxLargeQProducerIndex);
	}