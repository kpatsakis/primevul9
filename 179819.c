static int mwifiex_pcie_alloc_sleep_cookie_buf(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	u32 tmp;

	card->sleep_cookie_vbase = pci_alloc_consistent(card->dev, sizeof(u32),
						     &card->sleep_cookie_pbase);
	if (!card->sleep_cookie_vbase) {
		mwifiex_dbg(adapter, ERROR,
			    "pci_alloc_consistent failed!\n");
		return -ENOMEM;
	}
	/* Init val of Sleep Cookie */
	tmp = FW_AWAKE_COOKIE;
	put_unaligned(tmp, card->sleep_cookie_vbase);

	mwifiex_dbg(adapter, INFO,
		    "alloc_scook: sleep cookie=0x%x\n",
		    get_unaligned(card->sleep_cookie_vbase));

	return 0;
}