nfqnl_recv_config(struct sock *ctnl, struct sk_buff *skb,
		  const struct nlmsghdr *nlh,
		  const struct nlattr * const nfqa[])
{
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	u_int16_t queue_num = ntohs(nfmsg->res_id);
	struct nfqnl_instance *queue;
	struct nfqnl_msg_config_cmd *cmd = NULL;
	struct net *net = sock_net(ctnl);
	struct nfnl_queue_net *q = nfnl_queue_pernet(net);
	int ret = 0;

	if (nfqa[NFQA_CFG_CMD]) {
		cmd = nla_data(nfqa[NFQA_CFG_CMD]);

		/* Obsolete commands without queue context */
		switch (cmd->command) {
		case NFQNL_CFG_CMD_PF_BIND: return 0;
		case NFQNL_CFG_CMD_PF_UNBIND: return 0;
		}
	}

	rcu_read_lock();
	queue = instance_lookup(q, queue_num);
	if (queue && queue->peer_portid != NETLINK_CB(skb).portid) {
		ret = -EPERM;
		goto err_out_unlock;
	}

	if (cmd != NULL) {
		switch (cmd->command) {
		case NFQNL_CFG_CMD_BIND:
			if (queue) {
				ret = -EBUSY;
				goto err_out_unlock;
			}
			queue = instance_create(q, queue_num,
						NETLINK_CB(skb).portid);
			if (IS_ERR(queue)) {
				ret = PTR_ERR(queue);
				goto err_out_unlock;
			}
			break;
		case NFQNL_CFG_CMD_UNBIND:
			if (!queue) {
				ret = -ENODEV;
				goto err_out_unlock;
			}
			instance_destroy(q, queue);
			break;
		case NFQNL_CFG_CMD_PF_BIND:
		case NFQNL_CFG_CMD_PF_UNBIND:
			break;
		default:
			ret = -ENOTSUPP;
			break;
		}
	}

	if (nfqa[NFQA_CFG_PARAMS]) {
		struct nfqnl_msg_config_params *params;

		if (!queue) {
			ret = -ENODEV;
			goto err_out_unlock;
		}
		params = nla_data(nfqa[NFQA_CFG_PARAMS]);
		nfqnl_set_mode(queue, params->copy_mode,
				ntohl(params->copy_range));
	}

	if (nfqa[NFQA_CFG_QUEUE_MAXLEN]) {
		__be32 *queue_maxlen;

		if (!queue) {
			ret = -ENODEV;
			goto err_out_unlock;
		}
		queue_maxlen = nla_data(nfqa[NFQA_CFG_QUEUE_MAXLEN]);
		spin_lock_bh(&queue->lock);
		queue->queue_maxlen = ntohl(*queue_maxlen);
		spin_unlock_bh(&queue->lock);
	}

	if (nfqa[NFQA_CFG_FLAGS]) {
		__u32 flags, mask;

		if (!queue) {
			ret = -ENODEV;
			goto err_out_unlock;
		}

		if (!nfqa[NFQA_CFG_MASK]) {
			/* A mask is needed to specify which flags are being
			 * changed.
			 */
			ret = -EINVAL;
			goto err_out_unlock;
		}

		flags = ntohl(nla_get_be32(nfqa[NFQA_CFG_FLAGS]));
		mask = ntohl(nla_get_be32(nfqa[NFQA_CFG_MASK]));

		if (flags >= NFQA_CFG_F_MAX) {
			ret = -EOPNOTSUPP;
			goto err_out_unlock;
		}

		spin_lock_bh(&queue->lock);
		queue->flags &= ~mask;
		queue->flags |= flags & mask;
		spin_unlock_bh(&queue->lock);
	}

err_out_unlock:
	rcu_read_unlock();
	return ret;
}