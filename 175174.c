static void ovs_notify(struct genl_family *family,
		       struct sk_buff *skb, struct genl_info *info)
{
	genl_notify(family, skb, genl_info_net(info), info->snd_portid,
		    0, info->nlhdr, GFP_KERNEL);
}