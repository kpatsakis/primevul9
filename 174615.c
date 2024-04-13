static int ttusb_init_rc( struct ttusb_dec *dec)
{
	struct input_dev *input_dev;
	u8 b[] = { 0x00, 0x01 };
	int i;
	int err;

	usb_make_path(dec->udev, dec->rc_phys, sizeof(dec->rc_phys));
	strlcat(dec->rc_phys, "/input0", sizeof(dec->rc_phys));

	input_dev = input_allocate_device();
	if (!input_dev)
		return -ENOMEM;

	input_dev->name = "ttusb_dec remote control";
	input_dev->phys = dec->rc_phys;
	input_dev->evbit[0] = BIT_MASK(EV_KEY);
	input_dev->keycodesize = sizeof(u16);
	input_dev->keycodemax = 0x1a;
	input_dev->keycode = rc_keys;

	for (i = 0; i < ARRAY_SIZE(rc_keys); i++)
		  set_bit(rc_keys[i], input_dev->keybit);

	err = input_register_device(input_dev);
	if (err) {
		input_free_device(input_dev);
		return err;
	}

	dec->rc_input_dev = input_dev;
	if (usb_submit_urb(dec->irq_urb, GFP_KERNEL))
		printk("%s: usb_submit_urb failed\n",__func__);
	/* enable irq pipe */
	ttusb_dec_send_command(dec,0xb0,sizeof(b),b,NULL,NULL);

	return 0;
}