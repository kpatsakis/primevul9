isdn_net_ciscohdlck_disconnected(isdn_net_local *lp)
{
	del_timer(&lp->cisco_timer);
}