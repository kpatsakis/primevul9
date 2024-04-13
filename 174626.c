static void ttusb_dec_exit_filters(struct ttusb_dec *dec)
{
	struct list_head *item;
	struct filter_info *finfo;

	while ((item = dec->filter_info_list.next) != &dec->filter_info_list) {
		finfo = list_entry(item, struct filter_info, filter_info_list);
		list_del(&finfo->filter_info_list);
		kfree(finfo);
	}
}