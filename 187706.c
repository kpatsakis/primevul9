static inline void yam_rx_byte(struct net_device *dev, struct yam_port *yp, unsigned char rxb)
{
	if (yp->rx_len < YAM_MAX_FRAME) {
		unsigned char c = yp->rx_crcl;
		yp->rx_crcl = (chktabl[c] ^ yp->rx_crch);
		yp->rx_crch = (chktabh[c] ^ rxb);
		yp->rx_buf[yp->rx_len++] = rxb;
	}
}