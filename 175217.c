static int dp_register_genl(void)
{
	int n_registered;
	int err;
	int i;

	n_registered = 0;
	for (i = 0; i < ARRAY_SIZE(dp_genl_families); i++) {
		const struct genl_family_and_ops *f = &dp_genl_families[i];

		f->family->ops = f->ops;
		f->family->n_ops = f->n_ops;
		f->family->mcgrps = f->group;
		f->family->n_mcgrps = f->group ? 1 : 0;
		err = genl_register_family(f->family);
		if (err)
			goto error;
		n_registered++;
	}

	return 0;

error:
	dp_unregister_genl(n_registered);
	return err;
}