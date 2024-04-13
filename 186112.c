static void __init of_unittest_printf(void)
{
	struct device_node *np;
	const char *full_name = "/testcase-data/platform-tests/test-device@1/dev@100";
	char phandle_str[16] = "";

	np = of_find_node_by_path(full_name);
	if (!np) {
		unittest(np, "testcase data missing\n");
		return;
	}

	num_to_str(phandle_str, sizeof(phandle_str), np->phandle, 0);

	of_unittest_printf_one(np, "%pOF",  full_name);
	of_unittest_printf_one(np, "%pOFf", full_name);
	of_unittest_printf_one(np, "%pOFn", "dev");
	of_unittest_printf_one(np, "%2pOFn", "dev");
	of_unittest_printf_one(np, "%5pOFn", "  dev");
	of_unittest_printf_one(np, "%pOFnc", "dev:test-sub-device");
	of_unittest_printf_one(np, "%pOFp", phandle_str);
	of_unittest_printf_one(np, "%pOFP", "dev@100");
	of_unittest_printf_one(np, "ABC %pOFP ABC", "ABC dev@100 ABC");
	of_unittest_printf_one(np, "%10pOFP", "   dev@100");
	of_unittest_printf_one(np, "%-10pOFP", "dev@100   ");
	of_unittest_printf_one(of_root, "%pOFP", "/");
	of_unittest_printf_one(np, "%pOFF", "----");
	of_unittest_printf_one(np, "%pOFPF", "dev@100:----");
	of_unittest_printf_one(np, "%pOFPFPc", "dev@100:----:dev@100:test-sub-device");
	of_unittest_printf_one(np, "%pOFc", "test-sub-device");
	of_unittest_printf_one(np, "%pOFC",
			"\"test-sub-device\",\"test-compat2\",\"test-compat3\"");
}