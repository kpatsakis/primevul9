bool ovs_nla_get_ufid(struct sw_flow_id *sfid, const struct nlattr *attr,
		      bool log)
{
	sfid->ufid_len = get_ufid_len(attr, log);
	if (sfid->ufid_len)
		memcpy(sfid->ufid, nla_data(attr), sfid->ufid_len);

	return sfid->ufid_len;
}