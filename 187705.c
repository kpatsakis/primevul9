static void yam_start_tx(struct net_device *dev, struct yam_port *yp)
{
	if ((yp->tx_state == TX_TAIL) || (yp->txd == 0))
		yp->tx_count = 1;
	else
		yp->tx_count = (yp->bitrate * yp->txd) / 8000;
	yp->tx_state = TX_HEAD;
	ptt_on(dev);
}