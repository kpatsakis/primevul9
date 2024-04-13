static void ttusb_dec_exit_tasklet(struct ttusb_dec *dec)
{
	struct list_head *item;
	struct urb_frame *frame;

	tasklet_kill(&dec->urb_tasklet);

	while ((item = dec->urb_frame_list.next) != &dec->urb_frame_list) {
		frame = list_entry(item, struct urb_frame, urb_frame_list);
		list_del(&frame->urb_frame_list);
		kfree(frame);
	}
}