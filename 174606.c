static void ttusb_dec_process_urb_frame_list(unsigned long data)
{
	struct ttusb_dec *dec = (struct ttusb_dec *)data;
	struct list_head *item;
	struct urb_frame *frame;
	unsigned long flags;

	while (1) {
		spin_lock_irqsave(&dec->urb_frame_list_lock, flags);
		if ((item = dec->urb_frame_list.next) != &dec->urb_frame_list) {
			frame = list_entry(item, struct urb_frame,
					   urb_frame_list);
			list_del(&frame->urb_frame_list);
		} else {
			spin_unlock_irqrestore(&dec->urb_frame_list_lock,
					       flags);
			return;
		}
		spin_unlock_irqrestore(&dec->urb_frame_list_lock, flags);

		ttusb_dec_process_urb_frame(dec, frame->data, frame->length);
		kfree(frame);
	}
}