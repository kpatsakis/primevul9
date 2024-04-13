void af_cache_writethrough(AFFILE *af,int64_t pagenum,const unsigned char *buf,int bufflen)
{
    for(int i=0;i<af->num_pbufs;i++){
	struct aff_pagebuf *p = &af->pbcache[i];
	if(p->pagenum_valid && p->pagenum == pagenum){
	    if(p->pagebuf_dirty){
		(*af->error_reporter)("af_cache_writethrough: overwriting page %" I64u ".\n",pagenum);
		exit(-1);		// this shouldn't happen
	    }
	    memcpy(p->pagebuf,buf,bufflen);
	    memset(p->pagebuf+bufflen,0,af->image_pagesize-bufflen); // zero fill the rest
	    af->bytes_memcpy += bufflen;
	    p->pagebuf_valid = 1;	// we have a copy of it now.
	    p->pagebuf_dirty = 0;	// but it isn't dirty
	    p->last = cachetime++;
	}
    }
}