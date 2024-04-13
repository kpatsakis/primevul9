static int sas_check_level_subtractive_boundary(struct domain_device *dev)
{
	struct expander_device *ex = &dev->ex_dev;
	struct domain_device *child;
	u8 sub_addr[SAS_ADDR_SIZE] = {0, };

	list_for_each_entry(child, &ex->children, siblings) {
		if (child->dev_type != SAS_EDGE_EXPANDER_DEVICE &&
		    child->dev_type != SAS_FANOUT_EXPANDER_DEVICE)
			continue;
		if (sub_addr[0] == 0) {
			sas_find_sub_addr(child, sub_addr);
			continue;
		} else {
			u8 s2[SAS_ADDR_SIZE];

			if (sas_find_sub_addr(child, s2) &&
			    (SAS_ADDR(sub_addr) != SAS_ADDR(s2))) {

				pr_notice("ex %016llx->%016llx-?->%016llx diverges from subtractive boundary %016llx\n",
					  SAS_ADDR(dev->sas_addr),
					  SAS_ADDR(child->sas_addr),
					  SAS_ADDR(s2),
					  SAS_ADDR(sub_addr));

				sas_ex_disable_port(child, s2);
			}
		}
	}
	return 0;
}