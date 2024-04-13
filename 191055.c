int af_get_page_raw(AFFILE *af,int64_t pagenum,uint32_t *arg,
		    unsigned char *data,size_t *bytes)
{
    char segname[AF_MAX_NAME_LEN];

    memset(segname,0,sizeof(segname));
    sprintf(segname,AF_PAGE,pagenum);
    int r = af_get_seg(af,segname,arg,data,bytes);
    if(r < 0 && errno == ENOENT)
    {
	/* Couldn't read with AF_PAGE; try AF_SEG_D.
	 * This is legacy for the old AFF files. Perhaps we should delete it.
	 */
	sprintf(segname,AF_SEG_D,pagenum);
	r = af_get_seg(af,segname,arg,data,bytes);
    }
    /* Update the counters */
    if(r==0 && bytes && *bytes>0) af->pages_read++; // note that we read a page
    return r;
}