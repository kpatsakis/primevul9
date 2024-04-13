static void delayed_work_cb(struct work_struct *work)
{
	struct hidpp_device *hidpp = container_of(work, struct hidpp_device,
							work);
	hidpp_connect_event(hidpp);
}