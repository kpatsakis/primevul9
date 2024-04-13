static int ax88179_convert_old_led(struct usbnet *dev, u16 *ledvalue)
{
	u16 led;

	/* Loaded the old eFuse LED Mode */
	if (ax88179_read_cmd(dev, AX_ACCESS_EEPROM, 0x3C, 1, 2, &led) < 0)
		return -EINVAL;

	led >>= 8;
	switch (led) {
	case 0xFF:
		led = LED0_ACTIVE | LED1_LINK_10 | LED1_LINK_100 |
		      LED1_LINK_1000 | LED2_ACTIVE | LED2_LINK_10 |
		      LED2_LINK_100 | LED2_LINK_1000 | LED_VALID;
		break;
	case 0xFE:
		led = LED0_ACTIVE | LED1_LINK_1000 | LED2_LINK_100 | LED_VALID;
		break;
	case 0xFD:
		led = LED0_ACTIVE | LED1_LINK_1000 | LED2_LINK_100 |
		      LED2_LINK_10 | LED_VALID;
		break;
	case 0xFC:
		led = LED0_ACTIVE | LED1_ACTIVE | LED1_LINK_1000 | LED2_ACTIVE |
		      LED2_LINK_100 | LED2_LINK_10 | LED_VALID;
		break;
	default:
		led = LED0_ACTIVE | LED1_LINK_10 | LED1_LINK_100 |
		      LED1_LINK_1000 | LED2_ACTIVE | LED2_LINK_10 |
		      LED2_LINK_100 | LED2_LINK_1000 | LED_VALID;
		break;
	}

	*ledvalue = led;

	return 0;
}