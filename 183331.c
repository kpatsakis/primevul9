ptp_pack_OPL (PTPParams *params, MTPProperties *props, int nrofprops, unsigned char** opldataptr)
{
	unsigned char* opldata;
	MTPProperties *propitr;
	unsigned char *packedprops[MAX_MTP_PROPS];
	uint32_t packedpropslens[MAX_MTP_PROPS];
	uint32_t packedobjecthandles[MAX_MTP_PROPS];
	uint16_t packedpropsids[MAX_MTP_PROPS];
	uint16_t packedpropstypes[MAX_MTP_PROPS];
	uint32_t totalsize = 0;
	uint32_t bufp = 0;
	uint32_t noitems = 0;
	uint32_t i;

	totalsize = sizeof(uint32_t); /* 4 bytes to store the number of elements */
	propitr = props;
	while (nrofprops-- && noitems < MAX_MTP_PROPS) {
		/* Object Handle */
		packedobjecthandles[noitems]=propitr->ObjectHandle;
		totalsize += sizeof(uint32_t); /* Object ID */
		/* Metadata type */
		packedpropsids[noitems]=propitr->property;
		totalsize += sizeof(uint16_t);
		/* Data type */
		packedpropstypes[noitems]= propitr->datatype;
		totalsize += sizeof(uint16_t);
		/* Add each property to be sent. */
	        packedpropslens[noitems] = ptp_pack_DPV (params, &propitr->propval, &packedprops[noitems], propitr->datatype);
		totalsize += packedpropslens[noitems];
		noitems ++;
		propitr ++;
	}

	/* Allocate memory for the packed property list */
	opldata = malloc(totalsize);

	htod32a(&opldata[bufp],noitems);
	bufp += 4;

	/* Copy into a nice packed list */
	for (i = 0; i < noitems; i++) {
		/* Object ID */
		htod32a(&opldata[bufp],packedobjecthandles[i]);
		bufp += sizeof(uint32_t);
		htod16a(&opldata[bufp],packedpropsids[i]);
		bufp += sizeof(uint16_t);
		htod16a(&opldata[bufp],packedpropstypes[i]);
		bufp += sizeof(uint16_t);
		/* The copy the actual property */
		memcpy(&opldata[bufp], packedprops[i], packedpropslens[i]);
		bufp += packedpropslens[i];
		free(packedprops[i]);
	}
	*opldataptr = opldata;
	return totalsize;
}