xfrm_policy_flush_secctx_check(struct net *net, u8 type, bool task_valid)
{
	struct xfrm_policy *pol;
	int err = 0;

	list_for_each_entry(pol, &net->xfrm.policy_all, walk.all) {
		if (pol->walk.dead ||
		    xfrm_policy_id2dir(pol->index) >= XFRM_POLICY_MAX ||
		    pol->type != type)
			continue;

		err = security_xfrm_policy_delete(pol->security);
		if (err) {
			xfrm_audit_policy_delete(pol, 0, task_valid);
			return err;
		}
	}
	return err;
}