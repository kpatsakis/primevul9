int blkid_partitions_set_ptuuid(blkid_probe pr, unsigned char *uuid)
{
	struct blkid_chain *chn = blkid_probe_get_chain(pr);
	struct blkid_prval *v;

	if (chn->binary || blkid_uuid_is_empty(uuid, 16))
		return 0;

	v = blkid_probe_assign_value(pr, "PTUUID");
	if (!v)
		return -ENOMEM;

	v->len = 37;
	v->data = calloc(1, v->len);
	if (v->data) {
		blkid_unparse_uuid(uuid, (char *) v->data, v->len);
		return 0;
	}

	blkid_probe_free_value(v);
	return -ENOMEM;
}