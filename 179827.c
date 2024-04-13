static int mwifiex_pcie_probe_of(struct device *dev)
{
	if (!of_match_node(mwifiex_pcie_of_match_table, dev->of_node)) {
		dev_err(dev, "required compatible string missing\n");
		return -EINVAL;
	}

	return 0;
}