isdn_timer_funct(ulong dummy)
{
	int tf = dev->tflags;
	if (tf & ISDN_TIMER_FAST) {
		if (tf & ISDN_TIMER_MODEMREAD)
			isdn_tty_readmodem();
		if (tf & ISDN_TIMER_MODEMPLUS)
			isdn_tty_modem_escape();
		if (tf & ISDN_TIMER_MODEMXMIT)
			isdn_tty_modem_xmit();
	}
	if (tf & ISDN_TIMER_SLOW) {
		if (++isdn_timer_cnt1 >= ISDN_TIMER_02SEC) {
			isdn_timer_cnt1 = 0;
			if (tf & ISDN_TIMER_NETDIAL)
				isdn_net_dial();
		}
		if (++isdn_timer_cnt2 >= ISDN_TIMER_1SEC) {
			isdn_timer_cnt2 = 0;
			if (tf & ISDN_TIMER_NETHANGUP)
				isdn_net_autohup();
			if (++isdn_timer_cnt3 >= ISDN_TIMER_RINGING) {
				isdn_timer_cnt3 = 0;
				if (tf & ISDN_TIMER_MODEMRING)
					isdn_tty_modem_ring();
			}
			if (tf & ISDN_TIMER_CARRIER)
				isdn_tty_carrier_timeout();
		}
	}
	if (tf)
		mod_timer(&dev->timer, jiffies + ISDN_TIMER_RES);
}