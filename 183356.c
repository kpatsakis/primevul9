_lookup_or_allocate_canon_prop(PTPParams *params, uint16_t proptype)
{
	unsigned int j;

	for (j=0;j<params->nrofcanon_props;j++)
		if (params->canon_props[j].proptype == proptype)
			break;
	if (j<params->nrofcanon_props)
		return &params->canon_props[j].dpd;

	if (j)
		params->canon_props = realloc(params->canon_props, sizeof(params->canon_props[0])*(j+1));
	else
		params->canon_props = malloc(sizeof(params->canon_props[0]));
	params->canon_props[j].proptype = proptype;
	params->canon_props[j].size = 0;
	params->canon_props[j].data = NULL;
	memset (&params->canon_props[j].dpd,0,sizeof(params->canon_props[j].dpd));
	params->canon_props[j].dpd.GetSet = 1;
	params->canon_props[j].dpd.FormFlag = PTP_DPFF_None;
	params->nrofcanon_props = j+1;
	return &params->canon_props[j].dpd;
}