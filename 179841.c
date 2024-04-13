static int mwifiex_pcie_alloc_buffers(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	int ret;

	card->cmdrsp_buf = NULL;
	ret = mwifiex_pcie_create_txbd_ring(adapter);
	if (ret) {
		mwifiex_dbg(adapter, ERROR, "Failed to create txbd ring\n");
		goto err_cre_txbd;
	}

	ret = mwifiex_pcie_create_rxbd_ring(adapter);
	if (ret) {
		mwifiex_dbg(adapter, ERROR, "Failed to create rxbd ring\n");
		goto err_cre_rxbd;
	}

	ret = mwifiex_pcie_create_evtbd_ring(adapter);
	if (ret) {
		mwifiex_dbg(adapter, ERROR, "Failed to create evtbd ring\n");
		goto err_cre_evtbd;
	}

	ret = mwifiex_pcie_alloc_cmdrsp_buf(adapter);
	if (ret) {
		mwifiex_dbg(adapter, ERROR, "Failed to allocate cmdbuf buffer\n");
		goto err_alloc_cmdbuf;
	}

	if (reg->sleep_cookie) {
		ret = mwifiex_pcie_alloc_sleep_cookie_buf(adapter);
		if (ret) {
			mwifiex_dbg(adapter, ERROR, "Failed to allocate sleep_cookie buffer\n");
			goto err_alloc_cookie;
		}
	} else {
		card->sleep_cookie_vbase = NULL;
	}

	return 0;

err_alloc_cookie:
	mwifiex_pcie_delete_cmdrsp_buf(adapter);
err_alloc_cmdbuf:
	mwifiex_pcie_delete_evtbd_ring(adapter);
err_cre_evtbd:
	mwifiex_pcie_delete_rxbd_ring(adapter);
err_cre_rxbd:
	mwifiex_pcie_delete_txbd_ring(adapter);
err_cre_txbd:
	return ret;
}