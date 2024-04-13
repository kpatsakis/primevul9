generate_name(cms_context *cms, SECItem *der, CERTName *certname)
{
	void *marka = PORT_ArenaMark(cms->arena);
	CERTRDN **rdns = certname->rdns;
	CERTRDN *rdn;

	int num_items = 0;
	int rc = 0;

	while (rdns && (rdn = *rdns++) != NULL) {
		CERTAVA **avas = rdn->avas;
		CERTAVA *ava;
		while (avas && (ava = *avas++) != NULL)
			num_items++;
	}

	if (num_items == 0) {
		PORT_ArenaRelease(cms->arena, marka);
		cmsreterr(-1, cms, "No name items to encode");
	}

	SECItem items[num_items];

	int i = 0;
	rdns = certname->rdns;
	while (rdns && (rdn = *rdns++) != NULL) {
		CERTAVA **avas = rdn->avas;
		CERTAVA *ava;
		while (avas && (ava = *avas++) != NULL) {
			SECItem avader;
			rc = generate_ava(cms, &avader, ava);
			if (rc < 0) {
				PORT_ArenaRelease(cms->arena, marka);
				return -1;
			}

			SECItem *list[2] = {
				&avader,
				NULL,
			};
			rc = wrap_in_set(cms, &items[i], list);
			if (rc < 0) {
				PORT_ArenaRelease(cms->arena, marka);
				return -1;
			}
			i++;
		}
	}
	wrap_in_seq(cms, der, &items[0], num_items);
	PORT_ArenaUnmark(cms->arena, marka);

	return 0;
}