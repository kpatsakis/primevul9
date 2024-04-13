int af_alg_cmsg_send(struct msghdr *msg, struct af_alg_control *con)
{
	struct cmsghdr *cmsg;

	for_each_cmsghdr(cmsg, msg) {
		if (!CMSG_OK(msg, cmsg))
			return -EINVAL;
		if (cmsg->cmsg_level != SOL_ALG)
			continue;

		switch (cmsg->cmsg_type) {
		case ALG_SET_IV:
			if (cmsg->cmsg_len < CMSG_LEN(sizeof(*con->iv)))
				return -EINVAL;
			con->iv = (void *)CMSG_DATA(cmsg);
			if (cmsg->cmsg_len < CMSG_LEN(con->iv->ivlen +
						      sizeof(*con->iv)))
				return -EINVAL;
			break;

		case ALG_SET_OP:
			if (cmsg->cmsg_len < CMSG_LEN(sizeof(u32)))
				return -EINVAL;
			con->op = *(u32 *)CMSG_DATA(cmsg);
			break;

		case ALG_SET_AEAD_ASSOCLEN:
			if (cmsg->cmsg_len < CMSG_LEN(sizeof(u32)))
				return -EINVAL;
			con->aead_assoclen = *(u32 *)CMSG_DATA(cmsg);
			break;

		default:
			return -EINVAL;
		}
	}

	return 0;
}