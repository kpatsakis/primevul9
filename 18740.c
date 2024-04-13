static int xemaclite_mdio_wait(struct net_local *lp)
{
	u32 val;

	/* wait for the MDIO interface to not be busy or timeout
	 * after some time.
	 */
	return readx_poll_timeout(xemaclite_readl,
				  lp->base_addr + XEL_MDIOCTRL_OFFSET,
				  val, !(val & XEL_MDIOCTRL_MDIOSTS_MASK),
				  1000, 20000);
}