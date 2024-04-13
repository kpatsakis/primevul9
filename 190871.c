wc_create_gmap(wc_status *st)
{
    wc_gset *gset = st->ces_info->gset;
    wc_uchar *gset_ext = st->ces_info->gset_ext;
    int i, f;

    if (WcOption.strict_iso2022) {
	for (i = 0; i < WC_F_ISO_BASE; i++) {
	    cs94_gmap[i] = 0;
	    cs96_gmap[i] = 0;
	    cs94w_gmap[i] = 0;
	    cs96w_gmap[i] = 0;
	    cs942_gmap[i] = 0;
	}
    } else {
	for (i = 0; i < WC_F_ISO_BASE; i++) {
	    cs94_gmap[i] = gset_ext[0];
	    cs96_gmap[i] = gset_ext[1];
	    cs94w_gmap[i] = gset_ext[2];
	    cs96w_gmap[i] = gset_ext[3];
	    cs942_gmap[i] = gset_ext[0];
	}
    }
    for (i = 0; gset[i].ccs; i++) {
	f = WC_CCS_GET_F(gset[i].ccs) - WC_F_ISO_BASE;
	switch (WC_CCS_TYPE(gset[i].ccs)) {
	case WC_CCS_A_CS94:
	    switch (gset[i].ccs) {
	    case WC_CCS_JIS_X_0201K:
		if (!WcOption.use_jisx0201k)
		    continue;
		break;
	    }
	    cs94_gmap[f] = gset[i].g;
	    break;
	case WC_CCS_A_CS94W:
	    switch (gset[i].ccs) {
	    case WC_CCS_JIS_X_0212:
		if (!WcOption.use_jisx0212)
		    continue;
		break;
	    case WC_CCS_JIS_X_0213_1:
	    case WC_CCS_JIS_X_0213_2:
		if (!WcOption.use_jisx0213)
		    continue;
		break;
	    }
	    cs94w_gmap[f] = gset[i].g;
	    break;
	case WC_CCS_A_CS96:
	    cs96_gmap[f] = gset[i].g;
	    break;
	case WC_CCS_A_CS96W:
	    cs96w_gmap[f] = gset[i].g;
	    break;
	case WC_CCS_A_CS942:
	    cs942_gmap[f] = gset[i].g;
	    break;
	}
    }
}