void bnx2x_ilt_set_info(struct bnx2x *bp)
{
	struct ilt_client_info *ilt_client;
	struct bnx2x_ilt *ilt = BP_ILT(bp);
	u16 line = 0;

	ilt->start_line = FUNC_ILT_BASE(BP_FUNC(bp));
	DP(BNX2X_MSG_SP, "ilt starts at line %d\n", ilt->start_line);

	/* CDU */
	ilt_client = &ilt->clients[ILT_CLIENT_CDU];
	ilt_client->client_num = ILT_CLIENT_CDU;
	ilt_client->page_size = CDU_ILT_PAGE_SZ;
	ilt_client->flags = ILT_CLIENT_SKIP_MEM;
	ilt_client->start = line;
	line += bnx2x_cid_ilt_lines(bp);

	if (CNIC_SUPPORT(bp))
		line += CNIC_ILT_LINES;
	ilt_client->end = line - 1;

	DP(NETIF_MSG_IFUP, "ilt client[CDU]: start %d, end %d, psz 0x%x, flags 0x%x, hw psz %d\n",
	   ilt_client->start,
	   ilt_client->end,
	   ilt_client->page_size,
	   ilt_client->flags,
	   ilog2(ilt_client->page_size >> 12));

	/* QM */
	if (QM_INIT(bp->qm_cid_count)) {
		ilt_client = &ilt->clients[ILT_CLIENT_QM];
		ilt_client->client_num = ILT_CLIENT_QM;
		ilt_client->page_size = QM_ILT_PAGE_SZ;
		ilt_client->flags = 0;
		ilt_client->start = line;

		/* 4 bytes for each cid */
		line += DIV_ROUND_UP(bp->qm_cid_count * QM_QUEUES_PER_FUNC * 4,
							 QM_ILT_PAGE_SZ);

		ilt_client->end = line - 1;

		DP(NETIF_MSG_IFUP,
		   "ilt client[QM]: start %d, end %d, psz 0x%x, flags 0x%x, hw psz %d\n",
		   ilt_client->start,
		   ilt_client->end,
		   ilt_client->page_size,
		   ilt_client->flags,
		   ilog2(ilt_client->page_size >> 12));
	}

	if (CNIC_SUPPORT(bp)) {
		/* SRC */
		ilt_client = &ilt->clients[ILT_CLIENT_SRC];
		ilt_client->client_num = ILT_CLIENT_SRC;
		ilt_client->page_size = SRC_ILT_PAGE_SZ;
		ilt_client->flags = 0;
		ilt_client->start = line;
		line += SRC_ILT_LINES;
		ilt_client->end = line - 1;

		DP(NETIF_MSG_IFUP,
		   "ilt client[SRC]: start %d, end %d, psz 0x%x, flags 0x%x, hw psz %d\n",
		   ilt_client->start,
		   ilt_client->end,
		   ilt_client->page_size,
		   ilt_client->flags,
		   ilog2(ilt_client->page_size >> 12));

		/* TM */
		ilt_client = &ilt->clients[ILT_CLIENT_TM];
		ilt_client->client_num = ILT_CLIENT_TM;
		ilt_client->page_size = TM_ILT_PAGE_SZ;
		ilt_client->flags = 0;
		ilt_client->start = line;
		line += TM_ILT_LINES;
		ilt_client->end = line - 1;

		DP(NETIF_MSG_IFUP,
		   "ilt client[TM]: start %d, end %d, psz 0x%x, flags 0x%x, hw psz %d\n",
		   ilt_client->start,
		   ilt_client->end,
		   ilt_client->page_size,
		   ilt_client->flags,
		   ilog2(ilt_client->page_size >> 12));
	}

	BUG_ON(line > ILT_MAX_LINES);
}