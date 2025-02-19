static inline int cipso_v4_validate(const struct sk_buff *skb,
				    unsigned char **option)
{
	unsigned char *opt = *option;
	unsigned char err_offset = 0;
	u8 opt_len = opt[1];
	u8 opt_iter;
	u8 tag_len;

	if (opt_len < 8) {
		err_offset = 1;
		goto out;
	}

	if (get_unaligned_be32(&opt[2]) == 0) {
		err_offset = 2;
		goto out;
	}

	for (opt_iter = 6; opt_iter < opt_len;) {
		tag_len = opt[opt_iter + 1];
		if ((tag_len == 0) || (opt[opt_iter + 1] > (opt_len - opt_iter))) {
			err_offset = opt_iter + 1;
			goto out;
		}
		opt_iter += tag_len;
	}

out:
	*option = opt + err_offset;
	return err_offset;

}