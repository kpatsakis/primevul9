static int __init of_unittest(void)
{
	struct device_node *np;
	int res;

	/* adding data for unittest */

	if (IS_ENABLED(CONFIG_UML))
		unittest_unflatten_overlay_base();

	res = unittest_data_add();
	if (res)
		return res;
	if (!of_aliases)
		of_aliases = of_find_node_by_path("/aliases");

	np = of_find_node_by_path("/testcase-data/phandle-tests/consumer-a");
	if (!np) {
		pr_info("No testcase data in device tree; not running tests\n");
		return 0;
	}
	of_node_put(np);

	pr_info("start of unittest - you will see error messages\n");
	of_unittest_check_tree_linkage();
	of_unittest_check_phandles();
	of_unittest_find_node_by_name();
	of_unittest_dynamic();
	of_unittest_parse_phandle_with_args();
	of_unittest_parse_phandle_with_args_map();
	of_unittest_printf();
	of_unittest_property_string();
	of_unittest_property_copy();
	of_unittest_changeset();
	of_unittest_parse_interrupts();
	of_unittest_parse_interrupts_extended();
	of_unittest_match_node();
	of_unittest_platform_populate();
	of_unittest_overlay();

	/* Double check linkage after removing testcase data */
	of_unittest_check_tree_linkage();

	of_unittest_overlay_high_level();

	pr_info("end of unittest - %i passed, %i failed\n",
		unittest_results.passed, unittest_results.failed);

	return 0;
}