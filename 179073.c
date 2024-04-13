void peak_usb_init_time_ref(struct peak_time_ref *time_ref,
			    const struct peak_usb_adapter *adapter)
{
	if (time_ref) {
		memset(time_ref, 0, sizeof(struct peak_time_ref));
		time_ref->adapter = adapter;
	}
}