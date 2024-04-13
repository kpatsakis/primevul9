static int i40e_get_lump(struct i40e_pf *pf, struct i40e_lump_tracking *pile,
			 u16 needed, u16 id)
{
	int ret = -ENOMEM;
	int i, j;

	if (!pile || needed == 0 || id >= I40E_PILE_VALID_BIT) {
		dev_info(&pf->pdev->dev,
			 "param err: pile=%s needed=%d id=0x%04x\n",
			 pile ? "<valid>" : "<null>", needed, id);
		return -EINVAL;
	}

	/* start the linear search with an imperfect hint */
	i = pile->search_hint;
	while (i < pile->num_entries) {
		/* skip already allocated entries */
		if (pile->list[i] & I40E_PILE_VALID_BIT) {
			i++;
			continue;
		}

		/* do we have enough in this lump? */
		for (j = 0; (j < needed) && ((i+j) < pile->num_entries); j++) {
			if (pile->list[i+j] & I40E_PILE_VALID_BIT)
				break;
		}

		if (j == needed) {
			/* there was enough, so assign it to the requestor */
			for (j = 0; j < needed; j++)
				pile->list[i+j] = id | I40E_PILE_VALID_BIT;
			ret = i;
			pile->search_hint = i + j;
			break;
		}

		/* not enough, so skip over it and continue looking */
		i += j;
	}

	return ret;
}