static void __init of_unittest_overlay(void)
{
	struct device_node *bus_np = NULL;

	if (platform_driver_register(&unittest_driver)) {
		unittest(0, "could not register unittest driver\n");
		goto out;
	}

	bus_np = of_find_node_by_path(bus_path);
	if (bus_np == NULL) {
		unittest(0, "could not find bus_path \"%s\"\n", bus_path);
		goto out;
	}

	if (of_platform_default_populate(bus_np, NULL, NULL)) {
		unittest(0, "could not populate bus @ \"%s\"\n", bus_path);
		goto out;
	}

	if (!of_unittest_device_exists(100, PDEV_OVERLAY)) {
		unittest(0, "could not find unittest0 @ \"%s\"\n",
				unittest_path(100, PDEV_OVERLAY));
		goto out;
	}

	if (of_unittest_device_exists(101, PDEV_OVERLAY)) {
		unittest(0, "unittest1 @ \"%s\" should not exist\n",
				unittest_path(101, PDEV_OVERLAY));
		goto out;
	}

	unittest(1, "basic infrastructure of overlays passed");

	/* tests in sequence */
	of_unittest_overlay_0();
	of_unittest_overlay_1();
	of_unittest_overlay_2();
	of_unittest_overlay_3();
	of_unittest_overlay_4();
	of_unittest_overlay_5();
	of_unittest_overlay_6();
	of_unittest_overlay_8();

	of_unittest_overlay_10();
	of_unittest_overlay_11();

#if IS_BUILTIN(CONFIG_I2C)
	if (unittest(of_unittest_overlay_i2c_init() == 0, "i2c init failed\n"))
		goto out;

	of_unittest_overlay_i2c_12();
	of_unittest_overlay_i2c_13();
	of_unittest_overlay_i2c_14();
	of_unittest_overlay_i2c_15();

	of_unittest_overlay_i2c_cleanup();
#endif

	of_unittest_destroy_tracked_overlays();

out:
	of_node_put(bus_np);
}