static int bond_upper_dev_walk(struct net_device *upper,
			       struct netdev_nested_priv *priv)
{
	__be32 ip = *(__be32 *)priv->data;

	return ip == bond_confirm_addr(upper, 0, ip);
}