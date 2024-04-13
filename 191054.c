int af_cache_flush(AFFILE *af)
{
    if(af_trace) fprintf(af_trace,"af_cache_flush()\n");
    int ret = 0;
    for(int i=0;i<af->num_pbufs;i++){
	struct aff_pagebuf *p = &af->pbcache[i];
	if(p->pagebuf_valid && p->pagebuf_dirty){
	    if(af_update_page(af,p->pagenum,p->pagebuf,p->pagebuf_bytes)){
		ret = -1;		// got an error; keep going, though
	    }
	    p->pagebuf_dirty = 0;
	    if(af_trace) fprintf(af_trace,"af_cache_flush: slot %d page %" PRIu64 " flushed.\n",i,p->pagenum);
	}
    }
    return ret;				// now return the error that I might have gotten
}