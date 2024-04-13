static void __init of_unittest_dynamic(void)
{
	struct device_node *np;
	struct property *prop;

	np = of_find_node_by_path("/testcase-data");
	if (!np) {
		pr_err("missing testcase data\n");
		return;
	}

	/* Array of 4 properties for the purpose of testing */
	prop = kcalloc(4, sizeof(*prop), GFP_KERNEL);
	if (!prop) {
		unittest(0, "kzalloc() failed\n");
		return;
	}

	/* Add a new property - should pass*/
	prop->name = "new-property";
	prop->value = "new-property-data";
	prop->length = strlen(prop->value) + 1;
	unittest(of_add_property(np, prop) == 0, "Adding a new property failed\n");

	/* Try to add an existing property - should fail */
	prop++;
	prop->name = "new-property";
	prop->value = "new-property-data-should-fail";
	prop->length = strlen(prop->value) + 1;
	unittest(of_add_property(np, prop) != 0,
		 "Adding an existing property should have failed\n");

	/* Try to modify an existing property - should pass */
	prop->value = "modify-property-data-should-pass";
	prop->length = strlen(prop->value) + 1;
	unittest(of_update_property(np, prop) == 0,
		 "Updating an existing property should have passed\n");

	/* Try to modify non-existent property - should pass*/
	prop++;
	prop->name = "modify-property";
	prop->value = "modify-missing-property-data-should-pass";
	prop->length = strlen(prop->value) + 1;
	unittest(of_update_property(np, prop) == 0,
		 "Updating a missing property should have passed\n");

	/* Remove property - should pass */
	unittest(of_remove_property(np, prop) == 0,
		 "Removing a property should have passed\n");

	/* Adding very large property - should pass */
	prop++;
	prop->name = "large-property-PAGE_SIZEx8";
	prop->length = PAGE_SIZE * 8;
	prop->value = kzalloc(prop->length, GFP_KERNEL);
	unittest(prop->value != NULL, "Unable to allocate large buffer\n");
	if (prop->value)
		unittest(of_add_property(np, prop) == 0,
			 "Adding a large property should have passed\n");
}