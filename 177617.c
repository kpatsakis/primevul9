isdn_net_dial_req(isdn_net_local *lp)
{
	/* is there a better error code? */
	if (!(ISDN_NET_DIALMODE(*lp) == ISDN_NET_DM_AUTO)) return -EBUSY;

	return isdn_net_force_dial_lp(lp);
}