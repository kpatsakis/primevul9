void aarp_cleanup_module(void)
{
	del_timer_sync(&aarp_timer);
	unregister_netdevice_notifier(&aarp_notifier);
	unregister_snap_client(aarp_dl);
	aarp_purge();
}