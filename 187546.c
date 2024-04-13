static int sas_find_bcast_dev(struct domain_device *dev,
			      struct domain_device **src_dev)
{
	struct expander_device *ex = &dev->ex_dev;
	int ex_change_count = -1;
	int phy_id = -1;
	int res;
	struct domain_device *ch;

	res = sas_get_ex_change_count(dev, &ex_change_count);
	if (res)
		goto out;
	if (ex_change_count != -1 && ex_change_count != ex->ex_change_count) {
		/* Just detect if this expander phys phy change count changed,
		* in order to determine if this expander originate BROADCAST,
		* and do not update phy change count field in our structure.
		*/
		res = sas_find_bcast_phy(dev, &phy_id, 0, false);
		if (phy_id != -1) {
			*src_dev = dev;
			ex->ex_change_count = ex_change_count;
			pr_info("ex %016llx phy%02d change count has changed\n",
				SAS_ADDR(dev->sas_addr), phy_id);
			return res;
		} else
			pr_info("ex %016llx phys DID NOT change\n",
				SAS_ADDR(dev->sas_addr));
	}
	list_for_each_entry(ch, &ex->children, siblings) {
		if (ch->dev_type == SAS_EDGE_EXPANDER_DEVICE || ch->dev_type == SAS_FANOUT_EXPANDER_DEVICE) {
			res = sas_find_bcast_dev(ch, src_dev);
			if (*src_dev)
				return res;
		}
	}
out:
	return res;
}