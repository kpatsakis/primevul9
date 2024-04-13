static void bnx2x_init_tx_ring_one(struct bnx2x_fp_txdata *txdata)
{
	int i;

	for (i = 1; i <= NUM_TX_RINGS; i++) {
		struct eth_tx_next_bd *tx_next_bd =
			&txdata->tx_desc_ring[TX_DESC_CNT * i - 1].next_bd;

		tx_next_bd->addr_hi =
			cpu_to_le32(U64_HI(txdata->tx_desc_mapping +
				    BCM_PAGE_SIZE*(i % NUM_TX_RINGS)));
		tx_next_bd->addr_lo =
			cpu_to_le32(U64_LO(txdata->tx_desc_mapping +
				    BCM_PAGE_SIZE*(i % NUM_TX_RINGS)));
	}

	*txdata->tx_cons_sb = cpu_to_le16(0);

	SET_FLAG(txdata->tx_db.data.header.header, DOORBELL_HDR_DB_TYPE, 1);
	txdata->tx_db.data.zero_fill1 = 0;
	txdata->tx_db.data.prod = 0;

	txdata->tx_pkt_prod = 0;
	txdata->tx_pkt_cons = 0;
	txdata->tx_bd_prod = 0;
	txdata->tx_bd_cons = 0;
	txdata->tx_pkt = 0;
}