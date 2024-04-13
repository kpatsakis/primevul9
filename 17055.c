static int nft_value_init(const struct nft_ctx *ctx,
			  struct nft_data *data, struct nft_data_desc *desc,
			  const struct nlattr *nla)
{
	unsigned int len;

	len = nla_len(nla);
	if (len == 0)
		return -EINVAL;
	if (len > desc->size)
		return -EOVERFLOW;
	if (desc->len) {
		if (len != desc->len)
			return -EINVAL;
	} else {
		desc->len = len;
	}

	nla_memcpy(data->data, nla, len);

	return 0;
}