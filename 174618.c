static void ttusb_dec_init_filters(struct ttusb_dec *dec)
{
	INIT_LIST_HEAD(&dec->filter_info_list);
	spin_lock_init(&dec->filter_info_list_lock);
}