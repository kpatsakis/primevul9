generate_ava(cms_context *cms, SECItem *der, CERTAVA *certava)
{
	ava ava;

	SECOidData *oid;

	void *arena = PORT_NewArena(DER_DEFAULT_CHUNKSIZE);
	if (arena == NULL)
		cmsreterr(-1, cms, "could not create arena");

	void *real_arena = cms->arena;
	cms->arena = arena;

	oid = SECOID_FindOID(&certava->type);
	if (!oid) {
		save_port_err() {
			PORT_FreeArena(arena, PR_TRUE);
		}
		cms->arena = real_arena;
		cmsreterr(-1, cms, "could not find OID");
	}

	int rc = generate_object_id(cms, &ava.type, oid->offset);
	if (rc < 0) {
		PORT_FreeArena(arena, PR_TRUE);
		cms->arena = real_arena;
		return -1;
	}

	memcpy(&ava.value, &certava->value, sizeof (ava.value));

	void *ret;
	SECItem tmp;
	ret = SEC_ASN1EncodeItem(arena, &tmp, &ava, AVATemplate);
	if (ret == NULL) {
		save_port_err() {
			PORT_FreeArena(arena, PR_TRUE);
		}
		cms->arena = real_arena;
		cmsreterr(-1, cms, "could not encode AVA");
	}

	der->type = tmp.type;
	der->len = tmp.len;
	der->data = PORT_ArenaAlloc(real_arena, tmp.len);
	if (!der->data) {
		save_port_err() {
			PORT_FreeArena(arena, PR_TRUE);
		}
		cms->arena = real_arena;
		cmsreterr(-1, cms, "could not allocate AVA");
	}
	memcpy(der->data, tmp.data, tmp.len);
	PORT_FreeArena(arena, PR_TRUE);
	cms->arena = real_arena;

	return 0;
}