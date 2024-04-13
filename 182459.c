smb2_parse_lease_state(struct TCP_Server_Info *server,
		       struct smb2_create_rsp *rsp,
		       unsigned int *epoch, char *lease_key)
{
	char *data_offset;
	struct create_context *cc;
	unsigned int next;
	unsigned int remaining;
	char *name;

	data_offset = (char *)rsp + le32_to_cpu(rsp->CreateContextsOffset);
	remaining = le32_to_cpu(rsp->CreateContextsLength);
	cc = (struct create_context *)data_offset;
	while (remaining >= sizeof(struct create_context)) {
		name = le16_to_cpu(cc->NameOffset) + (char *)cc;
		if (le16_to_cpu(cc->NameLength) == 4 &&
		    strncmp(name, "RqLs", 4) == 0)
			return server->ops->parse_lease_buf(cc, epoch,
							    lease_key);

		next = le32_to_cpu(cc->Next);
		if (!next)
			break;
		remaining -= next;
		cc = (struct create_context *)((char *)cc + next);
	}

	return 0;
}