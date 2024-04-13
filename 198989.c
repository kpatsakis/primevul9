static int acm_submit_read_urb(struct acm *acm, int index, gfp_t mem_flags)
{
	int res;

	if (!test_and_clear_bit(index, &acm->read_urbs_free))
		return 0;

	res = usb_submit_urb(acm->read_urbs[index], mem_flags);
	if (res) {
		if (res != -EPERM && res != -ENODEV) {
			dev_err(&acm->data->dev,
				"urb %d failed submission with %d\n",
				index, res);
		}
		set_bit(index, &acm->read_urbs_free);
		return res;
	} else {
		dev_vdbg(&acm->data->dev, "submitted urb %d\n", index);
	}

	return 0;
}