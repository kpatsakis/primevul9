static struct slave *bond_xmit_3ad_xor_slave_get(struct bonding *bond,
						 struct sk_buff *skb,
						 struct bond_up_slave *slaves)
{
	struct slave *slave;
	unsigned int count;
	u32 hash;

	hash = bond_xmit_hash(bond, skb);
	count = slaves ? READ_ONCE(slaves->count) : 0;
	if (unlikely(!count))
		return NULL;

	slave = slaves->arr[hash % count];
	return slave;
}