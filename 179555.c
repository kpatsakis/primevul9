int af9005_led_control(struct dvb_usb_device *d, int onoff)
{
	struct af9005_device_state *st = d->priv;
	int temp, ret;

	if (onoff && dvb_usb_af9005_led)
		temp = 1;
	else
		temp = 0;
	if (st->led_state != temp) {
		ret =
		    af9005_write_register_bits(d, xd_p_reg_top_locken1,
					       reg_top_locken1_pos,
					       reg_top_locken1_len, temp);
		if (ret)
			return ret;
		ret =
		    af9005_write_register_bits(d, xd_p_reg_top_lock1,
					       reg_top_lock1_pos,
					       reg_top_lock1_len, temp);
		if (ret)
			return ret;
		st->led_state = temp;
	}
	return 0;
}