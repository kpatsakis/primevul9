static int crypto_add_alg(struct sk_buff *skb, struct nlmsghdr *nlh,
			  struct nlattr **attrs)
{
	int exact = 0;
	const char *name;
	struct crypto_alg *alg;
	struct crypto_user_alg *p = nlmsg_data(nlh);
	struct nlattr *priority = attrs[CRYPTOCFGA_PRIORITY_VAL];

	if (!netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if (!null_terminated(p->cru_name) || !null_terminated(p->cru_driver_name))
		return -EINVAL;

	if (strlen(p->cru_driver_name))
		exact = 1;

	if (priority && !exact)
		return -EINVAL;

	alg = crypto_alg_match(p, exact);
	if (alg) {
		crypto_mod_put(alg);
		return -EEXIST;
	}

	if (strlen(p->cru_driver_name))
		name = p->cru_driver_name;
	else
		name = p->cru_name;

	alg = crypto_alg_mod_lookup(name, p->cru_type, p->cru_mask);
	if (IS_ERR(alg))
		return PTR_ERR(alg);

	down_write(&crypto_alg_sem);

	if (priority)
		alg->cra_priority = nla_get_u32(priority);

	up_write(&crypto_alg_sem);

	crypto_mod_put(alg);

	return 0;
}