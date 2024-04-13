static int crypto_update_alg(struct sk_buff *skb, struct nlmsghdr *nlh,
			     struct nlattr **attrs)
{
	struct crypto_alg *alg;
	struct crypto_user_alg *p = nlmsg_data(nlh);
	struct nlattr *priority = attrs[CRYPTOCFGA_PRIORITY_VAL];
	LIST_HEAD(list);

	if (!netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if (!null_terminated(p->cru_name) || !null_terminated(p->cru_driver_name))
		return -EINVAL;

	if (priority && !strlen(p->cru_driver_name))
		return -EINVAL;

	alg = crypto_alg_match(p, 1);
	if (!alg)
		return -ENOENT;

	down_write(&crypto_alg_sem);

	crypto_remove_spawns(alg, &list, NULL);

	if (priority)
		alg->cra_priority = nla_get_u32(priority);

	up_write(&crypto_alg_sem);

	crypto_mod_put(alg);
	crypto_remove_final(&list);

	return 0;
}