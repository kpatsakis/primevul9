isdn_net_ciscohdlck_connected(isdn_net_local *lp)
{
	lp->cisco_myseq = 0;
	lp->cisco_mineseen = 0;
	lp->cisco_yourseq = 0;
	lp->cisco_keepalive_period = ISDN_TIMER_KEEPINT;
	lp->cisco_last_slarp_in = 0;
	lp->cisco_line_state = 0;
	lp->cisco_debserint = 0;

	/* send slarp request because interface/seq.no.s reset */
	isdn_net_ciscohdlck_slarp_send_request(lp);

	init_timer(&lp->cisco_timer);
	lp->cisco_timer.data = (unsigned long) lp;
	lp->cisco_timer.function = isdn_net_ciscohdlck_slarp_send_keepalive;
	lp->cisco_timer.expires = jiffies + lp->cisco_keepalive_period * HZ;
	add_timer(&lp->cisco_timer);
}