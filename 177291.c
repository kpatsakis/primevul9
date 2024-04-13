void sdma_clean(struct hfi1_devdata *dd, size_t num_engines)
{
	size_t i;
	struct sdma_engine *sde;

	if (dd->sdma_pad_dma) {
		dma_free_coherent(&dd->pcidev->dev, 4,
				  (void *)dd->sdma_pad_dma,
				  dd->sdma_pad_phys);
		dd->sdma_pad_dma = NULL;
		dd->sdma_pad_phys = 0;
	}
	if (dd->sdma_heads_dma) {
		dma_free_coherent(&dd->pcidev->dev, dd->sdma_heads_size,
				  (void *)dd->sdma_heads_dma,
				  dd->sdma_heads_phys);
		dd->sdma_heads_dma = NULL;
		dd->sdma_heads_phys = 0;
	}
	for (i = 0; dd->per_sdma && i < num_engines; ++i) {
		sde = &dd->per_sdma[i];

		sde->head_dma = NULL;
		sde->head_phys = 0;

		if (sde->descq) {
			dma_free_coherent(
				&dd->pcidev->dev,
				sde->descq_cnt * sizeof(u64[2]),
				sde->descq,
				sde->descq_phys
			);
			sde->descq = NULL;
			sde->descq_phys = 0;
		}
		kvfree(sde->tx_ring);
		sde->tx_ring = NULL;
	}
	spin_lock_irq(&dd->sde_map_lock);
	sdma_map_free(rcu_access_pointer(dd->sdma_map));
	RCU_INIT_POINTER(dd->sdma_map, NULL);
	spin_unlock_irq(&dd->sde_map_lock);
	synchronize_rcu();
	kfree(dd->per_sdma);
	dd->per_sdma = NULL;

	if (dd->sdma_rht) {
		rhashtable_free_and_destroy(dd->sdma_rht, sdma_rht_free, NULL);
		kfree(dd->sdma_rht);
		dd->sdma_rht = NULL;
	}
}