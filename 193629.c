struct crypto_alg *crypto_alg_match(struct crypto_user_alg *p, int exact)
{
	struct crypto_alg *q, *alg = NULL;

	down_read(&crypto_alg_sem);

	list_for_each_entry(q, &crypto_alg_list, cra_list) {
		int match = 0;

		if (crypto_is_larval(q))
			continue;

		if ((q->cra_flags ^ p->cru_type) & p->cru_mask)
			continue;

		if (strlen(p->cru_driver_name))
			match = !strcmp(q->cra_driver_name,
					p->cru_driver_name);
		else if (!exact)
			match = !strcmp(q->cra_name, p->cru_name);

		if (!match)
			continue;

		if (unlikely(!crypto_mod_get(q)))
			continue;

		alg = q;
		break;
	}

	up_read(&crypto_alg_sem);

	return alg;
}