static void mask_set_nlattr(struct nlattr *attr, u8 val)
{
	nlattr_set(attr, val, ovs_key_lens);
}