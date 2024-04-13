static int crypto_del_alg(struct sk_buff *skb, struct nlmsghdr *nlh,
			  struct nlattr **attrs)
{
	struct crypto_alg *alg;
	struct crypto_user_alg *p = nlmsg_data(nlh);
	int err;

	if (!netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if (!null_terminated(p->cru_name) || !null_terminated(p->cru_driver_name))
		return -EINVAL;

	alg = crypto_alg_match(p, 1);
	if (!alg)
		return -ENOENT;

	/* We can not unregister core algorithms such as aes-generic.
	 * We would loose the reference in the crypto_alg_list to this algorithm
	 * if we try to unregister. Unregistering such an algorithm without
	 * removing the module is not possible, so we restrict to crypto
	 * instances that are build from templates. */
	err = -EINVAL;
	if (!(alg->cra_flags & CRYPTO_ALG_INSTANCE))
		goto drop_alg;

	err = -EBUSY;
	if (refcount_read(&alg->cra_refcnt) > 2)
		goto drop_alg;

	err = crypto_unregister_instance((struct crypto_instance *)alg);

drop_alg:
	crypto_mod_put(alg);
	return err;
}