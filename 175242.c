static int ovs_vport_cmd_set(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr **a = info->attrs;
	struct sk_buff *reply;
	struct vport *vport;
	int err;

	ovs_lock();
	vport = lookup_vport(sock_net(skb->sk), info->userhdr, a);
	err = PTR_ERR(vport);
	if (IS_ERR(vport))
		goto exit_unlock;

	if (a[OVS_VPORT_ATTR_TYPE] &&
	    nla_get_u32(a[OVS_VPORT_ATTR_TYPE]) != vport->ops->type) {
		err = -EINVAL;
		goto exit_unlock;
	}

	reply = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!reply) {
		err = -ENOMEM;
		goto exit_unlock;
	}

	if (a[OVS_VPORT_ATTR_OPTIONS]) {
		err = ovs_vport_set_options(vport, a[OVS_VPORT_ATTR_OPTIONS]);
		if (err)
			goto exit_free;
	}

	if (a[OVS_VPORT_ATTR_UPCALL_PID])
		vport->upcall_portid = nla_get_u32(a[OVS_VPORT_ATTR_UPCALL_PID]);

	err = ovs_vport_cmd_fill_info(vport, reply, info->snd_portid,
				      info->snd_seq, 0, OVS_VPORT_CMD_NEW);
	BUG_ON(err < 0);

	ovs_unlock();
	ovs_notify(&dp_vport_genl_family, reply, info);
	return 0;

exit_free:
	kfree_skb(reply);
exit_unlock:
	ovs_unlock();
	return err;
}