static void __init of_unittest_platform_populate(void)
{
	int irq, rc;
	struct device_node *np, *child, *grandchild;
	struct platform_device *pdev, *test_bus;
	const struct of_device_id match[] = {
		{ .compatible = "test-device", },
		{}
	};

	np = of_find_node_by_path("/testcase-data");
	of_platform_default_populate(np, NULL, NULL);

	/* Test that a missing irq domain returns -EPROBE_DEFER */
	np = of_find_node_by_path("/testcase-data/testcase-device1");
	pdev = of_find_device_by_node(np);
	unittest(pdev, "device 1 creation failed\n");

	if (!(of_irq_workarounds & OF_IMAP_OLDWORLD_MAC)) {
		irq = platform_get_irq(pdev, 0);
		unittest(irq == -EPROBE_DEFER,
			 "device deferred probe failed - %d\n", irq);

		/* Test that a parsing failure does not return -EPROBE_DEFER */
		np = of_find_node_by_path("/testcase-data/testcase-device2");
		pdev = of_find_device_by_node(np);
		unittest(pdev, "device 2 creation failed\n");
		irq = platform_get_irq(pdev, 0);
		unittest(irq < 0 && irq != -EPROBE_DEFER,
			 "device parsing error failed - %d\n", irq);
	}

	np = of_find_node_by_path("/testcase-data/platform-tests");
	unittest(np, "No testcase data in device tree\n");
	if (!np)
		return;

	test_bus = platform_device_register_full(&test_bus_info);
	rc = PTR_ERR_OR_ZERO(test_bus);
	unittest(!rc, "testbus registration failed; rc=%i\n", rc);
	if (rc) {
		of_node_put(np);
		return;
	}
	test_bus->dev.of_node = np;

	/*
	 * Add a dummy resource to the test bus node after it is
	 * registered to catch problems with un-inserted resources. The
	 * DT code doesn't insert the resources, and it has caused the
	 * kernel to oops in the past. This makes sure the same bug
	 * doesn't crop up again.
	 */
	platform_device_add_resources(test_bus, &test_bus_res, 1);

	of_platform_populate(np, match, NULL, &test_bus->dev);
	for_each_child_of_node(np, child) {
		for_each_child_of_node(child, grandchild)
			unittest(of_find_device_by_node(grandchild),
				 "Could not create device for node '%pOFn'\n",
				 grandchild);
	}

	of_platform_depopulate(&test_bus->dev);
	for_each_child_of_node(np, child) {
		for_each_child_of_node(child, grandchild)
			unittest(!of_find_device_by_node(grandchild),
				 "device didn't get destroyed '%pOFn'\n",
				 grandchild);
	}

	platform_device_unregister(test_bus);
	of_node_put(np);
}