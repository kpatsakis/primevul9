struct aff_pagebuf *af_cache_alloc(AFFILE *af,int64_t pagenum)
{
    if(af_trace) fprintf(af_trace,"af_cache_alloc(%p,%" I64d ")\n",af,pagenum);

    /* Make sure nothing in the cache is dirty */
    if(af_cache_flush(af) < 0)
	return 0;

    /* See if this page is already in the cache */
    for(int i=0;i<af->num_pbufs;i++){
	struct aff_pagebuf *p = &af->pbcache[i];
	if(p->pagenum_valid && p->pagenum==pagenum){
	    af->cache_hits++;
	    if(af_trace) fprintf(af_trace,"  page %" I64d " satisfied fromcache\n",pagenum);
	    p->last = cachetime++;
	    return p;
	}
    }

    af->cache_misses++;
    int slot = -1;
    /* See if there is an empty slot in the cache */
    for(int i=0;i<af->num_pbufs;i++){
	struct aff_pagebuf *p = &af->pbcache[i];
	if(p->pagenum_valid==0){
	    slot = i;
	    if(af_trace) fprintf(af_trace,"  slot %d given to page %" I64d "\n",slot,pagenum);
	    break;
	}
    }
    if(slot==-1){
	/* Find the oldest cache entry */
	int oldest_i = 0;
	int oldest_t = af->pbcache[0].last;
	for(int i=1;i<af->num_pbufs;i++){
	    if(af->pbcache[i].last < oldest_t){
		oldest_t = af->pbcache[i].last;
		oldest_i = i;
	    }
	}
	slot = oldest_i;
	if(af_trace) fprintf(af_trace,"  slot %d assigned to page %" I64d "\n",slot,pagenum);
    }
    /* take over this slot */
    struct aff_pagebuf *p = &af->pbcache[slot];
    if(p->pagebuf==0){
	p->pagebuf = (unsigned char *)valloc(af->image_pagesize); // allocate to a page boundary
	if(p->pagebuf==0){
	    /* Malloc failed; See if we can just use the first slot */
	    slot = 0;
	    if(af->pbcache[0].pagebuf==0) return 0; // ugh. Cannot malloc?

	    /* First slot is available. Just use it. */
	    p = &af->pbcache[0];
	}
    }
    memset(p->pagebuf,0,af->image_pagesize); // clean object reuse
    p->pagenum = pagenum;
    p->pagenum_valid = 1;
    p->pagebuf_valid = 0;
    p->pagebuf_dirty = 0;
    p->last = cachetime++;
    if(af_trace){
	fprintf(af_trace,"   current pages in cache: ");
	for(int i=0;i<af->num_pbufs;i++){
	    fprintf(af_trace," %" I64d,af->pbcache[i].pagenum);
	}
	fprintf(af_trace,"\n");
    }
    return p;
}