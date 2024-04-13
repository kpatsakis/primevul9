static irqreturn_t mwifiex_pcie_interrupt(int irq, void *context)
{
	struct mwifiex_msix_context *ctx = context;
	struct pci_dev *pdev = ctx->dev;
	struct pcie_service_card *card;
	struct mwifiex_adapter *adapter;

	card = pci_get_drvdata(pdev);

	if (!card->adapter) {
		pr_err("info: %s: card=%p adapter=%p\n", __func__, card,
		       card ? card->adapter : NULL);
		goto exit;
	}
	adapter = card->adapter;

	if (test_bit(MWIFIEX_SURPRISE_REMOVED, &adapter->work_flags))
		goto exit;

	if (card->msix_enable)
		mwifiex_interrupt_status(adapter, ctx->msg_id);
	else
		mwifiex_interrupt_status(adapter, -1);

	mwifiex_queue_main_work(adapter);

exit:
	return IRQ_HANDLED;
}