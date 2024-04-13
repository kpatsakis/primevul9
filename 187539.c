static void ex_assign_report_general(struct domain_device *dev,
					    struct smp_resp *resp)
{
	struct report_general_resp *rg = &resp->rg;

	dev->ex_dev.ex_change_count = be16_to_cpu(rg->change_count);
	dev->ex_dev.max_route_indexes = be16_to_cpu(rg->route_indexes);
	dev->ex_dev.num_phys = min(rg->num_phys, (u8)MAX_EXPANDER_PHYS);
	dev->ex_dev.t2t_supp = rg->t2t_supp;
	dev->ex_dev.conf_route_table = rg->conf_route_table;
	dev->ex_dev.configuring = rg->configuring;
	memcpy(dev->ex_dev.enclosure_logical_id, rg->enclosure_logical_id, 8);
}