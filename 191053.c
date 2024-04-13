int af_set_pagesize(AFFILE *af,uint32_t pagesize)
{
    /* Allow the pagesize to be changed if it hasn't been set yet
     * and if this format doesn't support metadata updating (which is the raw formats)
     */
    struct af_vnode_info vni;

    af_vstat(af,&vni);

    if(vni.changable_pagesize==0 && af->image_size>0){
	if(pagesize==af->image_pagesize) return 0; // it's already set to this, so let it pass
	errno = EINVAL;
	return -1;
    }
    if(pagesize % af->image_sectorsize != 0){
	(*af->error_reporter)("Cannot set pagesize to %d (sectorsize=%d)\n",
			      pagesize,af->image_sectorsize);
	errno = EINVAL;
	return -1;
    }

    af->image_pagesize = pagesize;
    if(af_update_seg(af,AF_PAGESIZE,pagesize,0,0)){
	if(errno != ENOTSUP) return -1;	// error updating (don't report ENOTSUP);
    }
    return 0;
}