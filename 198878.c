wc_get_ucs_table(wc_ccs ccs)
{
    int f = WC_CCS_INDEX(ccs);

    switch (WC_CCS_TYPE(ccs)) {
    case WC_CCS_A_CS94:
	if (f < WC_F_ISO_BASE || f > WC_F_CS94_END)
	    return NULL;
	return &ucs_cs94_table[f - WC_F_ISO_BASE];
    case WC_CCS_A_CS94W:
	if (f < WC_F_ISO_BASE || f > WC_F_CS94W_END)
	    return NULL;
	return &ucs_cs94w_table[f - WC_F_ISO_BASE];
    case WC_CCS_A_CS96:
	if (f < WC_F_ISO_BASE || f > WC_F_CS96_END)
	    return NULL;
	return &ucs_cs96_table[f - WC_F_ISO_BASE];
    case WC_CCS_A_CS96W:
	if (f < WC_F_ISO_BASE || f > WC_F_CS96W_END)
	    return NULL;
	return &ucs_cs96w_table[f - WC_F_ISO_BASE];
    case WC_CCS_A_CS942:
	if (f < WC_F_ISO_BASE || f > WC_F_CS942_END)
	    return NULL;
	return &ucs_cs942_table[f - WC_F_ISO_BASE];
    case WC_CCS_A_PCS:
	if (f < WC_F_PCS_BASE || f > WC_F_PCS_END)
	    return NULL;
	return &ucs_pcs_table[f - WC_F_PCS_BASE];
    case WC_CCS_A_PCSW:
	if (f < WC_F_PCS_BASE || f > WC_F_PCSW_END)
	    return NULL;
	return &ucs_pcsw_table[f - WC_F_PCS_BASE];
    default:
	return NULL;
    }
}