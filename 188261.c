static irqreturn_t ll_temac_tx_irq(int irq, void *_ndev)
{
	struct net_device *ndev = _ndev;
	struct temac_local *lp = netdev_priv(ndev);
	unsigned int status;

	status = lp->dma_in(lp, TX_IRQ_REG);
	lp->dma_out(lp, TX_IRQ_REG, status);

	if (status & (IRQ_COAL | IRQ_DLY))
		temac_start_xmit_done(lp->ndev);
	if (status & (IRQ_ERR | IRQ_DMAERR))
		dev_err_ratelimited(&ndev->dev,
				    "TX error 0x%x TX_CHNL_STS=0x%08x\n",
				    status, lp->dma_in(lp, TX_CHNL_STS));

	return IRQ_HANDLED;
}