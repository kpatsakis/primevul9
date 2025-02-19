int sdma_init(struct hfi1_devdata *dd, u8 port)
{
	unsigned this_idx;
	struct sdma_engine *sde;
	struct rhashtable *tmp_sdma_rht;
	u16 descq_cnt;
	void *curr_head;
	struct hfi1_pportdata *ppd = dd->pport + port;
	u32 per_sdma_credits;
	uint idle_cnt = sdma_idle_cnt;
	size_t num_engines = chip_sdma_engines(dd);
	int ret = -ENOMEM;

	if (!HFI1_CAP_IS_KSET(SDMA)) {
		HFI1_CAP_CLEAR(SDMA_AHG);
		return 0;
	}
	if (mod_num_sdma &&
	    /* can't exceed chip support */
	    mod_num_sdma <= chip_sdma_engines(dd) &&
	    /* count must be >= vls */
	    mod_num_sdma >= num_vls)
		num_engines = mod_num_sdma;

	dd_dev_info(dd, "SDMA mod_num_sdma: %u\n", mod_num_sdma);
	dd_dev_info(dd, "SDMA chip_sdma_engines: %u\n", chip_sdma_engines(dd));
	dd_dev_info(dd, "SDMA chip_sdma_mem_size: %u\n",
		    chip_sdma_mem_size(dd));

	per_sdma_credits =
		chip_sdma_mem_size(dd) / (num_engines * SDMA_BLOCK_SIZE);

	/* set up freeze waitqueue */
	init_waitqueue_head(&dd->sdma_unfreeze_wq);
	atomic_set(&dd->sdma_unfreeze_count, 0);

	descq_cnt = sdma_get_descq_cnt();
	dd_dev_info(dd, "SDMA engines %zu descq_cnt %u\n",
		    num_engines, descq_cnt);

	/* alloc memory for array of send engines */
	dd->per_sdma = kcalloc_node(num_engines, sizeof(*dd->per_sdma),
				    GFP_KERNEL, dd->node);
	if (!dd->per_sdma)
		return ret;

	idle_cnt = ns_to_cclock(dd, idle_cnt);
	if (idle_cnt)
		dd->default_desc1 =
			SDMA_DESC1_HEAD_TO_HOST_FLAG;
	else
		dd->default_desc1 =
			SDMA_DESC1_INT_REQ_FLAG;

	if (!sdma_desct_intr)
		sdma_desct_intr = SDMA_DESC_INTR;

	/* Allocate memory for SendDMA descriptor FIFOs */
	for (this_idx = 0; this_idx < num_engines; ++this_idx) {
		sde = &dd->per_sdma[this_idx];
		sde->dd = dd;
		sde->ppd = ppd;
		sde->this_idx = this_idx;
		sde->descq_cnt = descq_cnt;
		sde->desc_avail = sdma_descq_freecnt(sde);
		sde->sdma_shift = ilog2(descq_cnt);
		sde->sdma_mask = (1 << sde->sdma_shift) - 1;

		/* Create a mask specifically for each interrupt source */
		sde->int_mask = (u64)1 << (0 * TXE_NUM_SDMA_ENGINES +
					   this_idx);
		sde->progress_mask = (u64)1 << (1 * TXE_NUM_SDMA_ENGINES +
						this_idx);
		sde->idle_mask = (u64)1 << (2 * TXE_NUM_SDMA_ENGINES +
					    this_idx);
		/* Create a combined mask to cover all 3 interrupt sources */
		sde->imask = sde->int_mask | sde->progress_mask |
			     sde->idle_mask;

		spin_lock_init(&sde->tail_lock);
		seqlock_init(&sde->head_lock);
		spin_lock_init(&sde->senddmactrl_lock);
		spin_lock_init(&sde->flushlist_lock);
		seqlock_init(&sde->waitlock);
		/* insure there is always a zero bit */
		sde->ahg_bits = 0xfffffffe00000000ULL;

		sdma_set_state(sde, sdma_state_s00_hw_down);

		/* set up reference counting */
		kref_init(&sde->state.kref);
		init_completion(&sde->state.comp);

		INIT_LIST_HEAD(&sde->flushlist);
		INIT_LIST_HEAD(&sde->dmawait);

		sde->tail_csr =
			get_kctxt_csr_addr(dd, this_idx, SD(TAIL));

		tasklet_init(&sde->sdma_hw_clean_up_task, sdma_hw_clean_up_task,
			     (unsigned long)sde);

		tasklet_init(&sde->sdma_sw_clean_up_task, sdma_sw_clean_up_task,
			     (unsigned long)sde);
		INIT_WORK(&sde->err_halt_worker, sdma_err_halt_wait);
		INIT_WORK(&sde->flush_worker, sdma_field_flush);

		sde->progress_check_head = 0;

		timer_setup(&sde->err_progress_check_timer,
			    sdma_err_progress_check, 0);

		sde->descq = dma_alloc_coherent(&dd->pcidev->dev,
						descq_cnt * sizeof(u64[2]),
						&sde->descq_phys, GFP_KERNEL);
		if (!sde->descq)
			goto bail;
		sde->tx_ring =
			kvzalloc_node(array_size(descq_cnt,
						 sizeof(struct sdma_txreq *)),
				      GFP_KERNEL, dd->node);
		if (!sde->tx_ring)
			goto bail;
	}

	dd->sdma_heads_size = L1_CACHE_BYTES * num_engines;
	/* Allocate memory for DMA of head registers to memory */
	dd->sdma_heads_dma = dma_alloc_coherent(&dd->pcidev->dev,
						dd->sdma_heads_size,
						&dd->sdma_heads_phys,
						GFP_KERNEL);
	if (!dd->sdma_heads_dma) {
		dd_dev_err(dd, "failed to allocate SendDMA head memory\n");
		goto bail;
	}

	/* Allocate memory for pad */
	dd->sdma_pad_dma = dma_alloc_coherent(&dd->pcidev->dev, sizeof(u32),
					      &dd->sdma_pad_phys, GFP_KERNEL);
	if (!dd->sdma_pad_dma) {
		dd_dev_err(dd, "failed to allocate SendDMA pad memory\n");
		goto bail;
	}

	/* assign each engine to different cacheline and init registers */
	curr_head = (void *)dd->sdma_heads_dma;
	for (this_idx = 0; this_idx < num_engines; ++this_idx) {
		unsigned long phys_offset;

		sde = &dd->per_sdma[this_idx];

		sde->head_dma = curr_head;
		curr_head += L1_CACHE_BYTES;
		phys_offset = (unsigned long)sde->head_dma -
			      (unsigned long)dd->sdma_heads_dma;
		sde->head_phys = dd->sdma_heads_phys + phys_offset;
		init_sdma_regs(sde, per_sdma_credits, idle_cnt);
	}
	dd->flags |= HFI1_HAS_SEND_DMA;
	dd->flags |= idle_cnt ? HFI1_HAS_SDMA_TIMEOUT : 0;
	dd->num_sdma = num_engines;
	ret = sdma_map_init(dd, port, ppd->vls_operational, NULL);
	if (ret < 0)
		goto bail;

	tmp_sdma_rht = kzalloc(sizeof(*tmp_sdma_rht), GFP_KERNEL);
	if (!tmp_sdma_rht) {
		ret = -ENOMEM;
		goto bail;
	}

	ret = rhashtable_init(tmp_sdma_rht, &sdma_rht_params);
	if (ret < 0) {
		kfree(tmp_sdma_rht);
		goto bail;
	}

	dd->sdma_rht = tmp_sdma_rht;

	dd_dev_info(dd, "SDMA num_sdma: %u\n", dd->num_sdma);
	return 0;

bail:
	sdma_clean(dd, num_engines);
	return ret;
}