static int cipso_v4_map_lvl_valid(const struct cipso_v4_doi *doi_def, u8 level)
{
	switch (doi_def->type) {
	case CIPSO_V4_MAP_PASS:
		return 0;
	case CIPSO_V4_MAP_TRANS:
		if (doi_def->map.std->lvl.cipso[level] < CIPSO_V4_INV_LVL)
			return 0;
		break;
	}

	return -EFAULT;
}