static int sas_find_bcast_phy(struct domain_device *dev, int *phy_id,
			      int from_phy, bool update)
{
	struct expander_device *ex = &dev->ex_dev;
	int res = 0;
	int i;

	for (i = from_phy; i < ex->num_phys; i++) {
		int phy_change_count = 0;

		res = sas_get_phy_change_count(dev, i, &phy_change_count);
		switch (res) {
		case SMP_RESP_PHY_VACANT:
		case SMP_RESP_NO_PHY:
			continue;
		case SMP_RESP_FUNC_ACC:
			break;
		default:
			return res;
		}

		if (phy_change_count != ex->ex_phy[i].phy_change_count) {
			if (update)
				ex->ex_phy[i].phy_change_count =
					phy_change_count;
			*phy_id = i;
			return 0;
		}
	}
	return 0;
}