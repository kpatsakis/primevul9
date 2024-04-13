static int sony_leds_init(struct sony_sc *sc)
{
	struct hid_device *hdev = sc->hdev;
	int n, ret = 0;
	int use_ds4_names;
	struct led_classdev *led;
	size_t name_sz;
	char *name;
	size_t name_len;
	const char *name_fmt;
	static const char * const ds4_name_str[] = { "red", "green", "blue",
						  "global" };
	u8 max_brightness[MAX_LEDS] = { [0 ... (MAX_LEDS - 1)] = 1 };
	u8 use_hw_blink[MAX_LEDS] = { 0 };

	BUG_ON(!(sc->quirks & SONY_LED_SUPPORT));

	if (sc->quirks & BUZZ_CONTROLLER) {
		sc->led_count = 4;
		use_ds4_names = 0;
		name_len = strlen("::buzz#");
		name_fmt = "%s::buzz%d";
		/* Validate expected report characteristics. */
		if (!hid_validate_values(hdev, HID_OUTPUT_REPORT, 0, 0, 7))
			return -ENODEV;
	} else if (sc->quirks & DUALSHOCK4_CONTROLLER) {
		dualshock4_set_leds_from_id(sc);
		sc->led_state[3] = 1;
		sc->led_count = 4;
		memset(max_brightness, 255, 3);
		use_hw_blink[3] = 1;
		use_ds4_names = 1;
		name_len = 0;
		name_fmt = "%s:%s";
	} else if (sc->quirks & MOTION_CONTROLLER) {
		sc->led_count = 3;
		memset(max_brightness, 255, 3);
		use_ds4_names = 1;
		name_len = 0;
		name_fmt = "%s:%s";
	} else if (sc->quirks & NAVIGATION_CONTROLLER) {
		static const u8 navigation_leds[4] = {0x01, 0x00, 0x00, 0x00};

		memcpy(sc->led_state, navigation_leds, sizeof(navigation_leds));
		sc->led_count = 1;
		memset(use_hw_blink, 1, 4);
		use_ds4_names = 0;
		name_len = strlen("::sony#");
		name_fmt = "%s::sony%d";
	} else {
		sixaxis_set_leds_from_id(sc);
		sc->led_count = 4;
		memset(use_hw_blink, 1, 4);
		use_ds4_names = 0;
		name_len = strlen("::sony#");
		name_fmt = "%s::sony%d";
	}

	/*
	 * Clear LEDs as we have no way of reading their initial state. This is
	 * only relevant if the driver is loaded after somebody actively set the
	 * LEDs to on
	 */
	sony_set_leds(sc);

	name_sz = strlen(dev_name(&hdev->dev)) + name_len + 1;

	for (n = 0; n < sc->led_count; n++) {

		if (use_ds4_names)
			name_sz = strlen(dev_name(&hdev->dev)) + strlen(ds4_name_str[n]) + 2;

		led = devm_kzalloc(&hdev->dev, sizeof(struct led_classdev) + name_sz, GFP_KERNEL);
		if (!led) {
			hid_err(hdev, "Couldn't allocate memory for LED %d\n", n);
			return -ENOMEM;
		}

		name = (void *)(&led[1]);
		if (use_ds4_names)
			snprintf(name, name_sz, name_fmt, dev_name(&hdev->dev),
			ds4_name_str[n]);
		else
			snprintf(name, name_sz, name_fmt, dev_name(&hdev->dev), n + 1);
		led->name = name;
		led->brightness = sc->led_state[n];
		led->max_brightness = max_brightness[n];
		led->flags = LED_CORE_SUSPENDRESUME;
		led->brightness_get = sony_led_get_brightness;
		led->brightness_set = sony_led_set_brightness;

		if (use_hw_blink[n])
			led->blink_set = sony_led_blink_set;

		sc->leds[n] = led;

		ret = devm_led_classdev_register(&hdev->dev, led);
		if (ret) {
			hid_err(hdev, "Failed to register LED %d\n", n);
			return ret;
		}
	}

	return 0;
}