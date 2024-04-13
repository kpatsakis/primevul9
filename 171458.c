
static int ql_alloc_small_buffers(struct ql3_adapter *qdev)
{
	int i;
	struct bufq_addr_element *small_buf_q_entry;

	/* Currently we allocate on one of memory and use it for smallbuffers */
	qdev->small_buf_total_size =
		(QL_ADDR_ELE_PER_BUFQ_ENTRY * NUM_SBUFQ_ENTRIES *
		 QL_SMALL_BUFFER_SIZE);

	qdev->small_buf_virt_addr =
		pci_alloc_consistent(qdev->pdev,
				     qdev->small_buf_total_size,
				     &qdev->small_buf_phy_addr);

	if (qdev->small_buf_virt_addr == NULL) {
		netdev_err(qdev->ndev, "Failed to get small buffer memory\n");
		return -ENOMEM;
	}

	qdev->small_buf_phy_addr_low = LS_64BITS(qdev->small_buf_phy_addr);
	qdev->small_buf_phy_addr_high = MS_64BITS(qdev->small_buf_phy_addr);

	small_buf_q_entry = qdev->small_buf_q_virt_addr;

	/* Initialize the small buffer queue. */
	for (i = 0; i < (QL_ADDR_ELE_PER_BUFQ_ENTRY * NUM_SBUFQ_ENTRIES); i++) {
		small_buf_q_entry->addr_high =
		    cpu_to_le32(qdev->small_buf_phy_addr_high);
		small_buf_q_entry->addr_low =
		    cpu_to_le32(qdev->small_buf_phy_addr_low +
				(i * QL_SMALL_BUFFER_SIZE));
		small_buf_q_entry++;
	}
	qdev->small_buf_index = 0;
	set_bit(QL_ALLOC_SMALL_BUF_DONE, &qdev->flags);
	return 0;