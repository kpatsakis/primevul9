static int hidp_input_event(struct input_dev *dev, unsigned int type,
			    unsigned int code, int value)
{
	struct hidp_session *session = input_get_drvdata(dev);
	unsigned char newleds;
	unsigned char hdr, data[2];

	BT_DBG("session %p type %d code %d value %d",
	       session, type, code, value);

	if (type != EV_LED)
		return -1;

	newleds = (!!test_bit(LED_KANA,    dev->led) << 3) |
		  (!!test_bit(LED_COMPOSE, dev->led) << 3) |
		  (!!test_bit(LED_SCROLLL, dev->led) << 2) |
		  (!!test_bit(LED_CAPSL,   dev->led) << 1) |
		  (!!test_bit(LED_NUML,    dev->led) << 0);

	if (session->leds == newleds)
		return 0;

	session->leds = newleds;

	hdr = HIDP_TRANS_DATA | HIDP_DATA_RTYPE_OUPUT;
	data[0] = 0x01;
	data[1] = newleds;

	return hidp_send_intr_message(session, hdr, data, 2);
}