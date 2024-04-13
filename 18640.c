int xfrm_policy_flush(struct net *net, u8 type, bool task_valid)
{
	int dir, err = 0, cnt = 0;
	struct xfrm_policy *pol;

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);

	err = xfrm_policy_flush_secctx_check(net, type, task_valid);
	if (err)
		goto out;

again:
	list_for_each_entry(pol, &net->xfrm.policy_all, walk.all) {
		dir = xfrm_policy_id2dir(pol->index);
		if (pol->walk.dead ||
		    dir >= XFRM_POLICY_MAX ||
		    pol->type != type)
			continue;

		__xfrm_policy_unlink(pol, dir);
		spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
		cnt++;
		xfrm_audit_policy_delete(pol, 1, task_valid);
		xfrm_policy_kill(pol);
		spin_lock_bh(&net->xfrm.xfrm_policy_lock);
		goto again;
	}
	if (cnt)
		__xfrm_policy_inexact_flush(net);
	else
		err = -ESRCH;
out:
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
	return err;
}