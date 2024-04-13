static void ttusb_dec_init_tasklet(struct ttusb_dec *dec)
{
	spin_lock_init(&dec->urb_frame_list_lock);
	INIT_LIST_HEAD(&dec->urb_frame_list);
	tasklet_init(&dec->urb_tasklet, ttusb_dec_process_urb_frame_list,
		     (unsigned long)dec);
}