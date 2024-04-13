int af_set_sectorsize(AFFILE *af,int sectorsize)
{
    struct af_vnode_info vni;
    af_vstat(af,&vni);
    if(vni.changable_pagesize==0 && af->image_size>0){
	errno = EINVAL;
	return -1;
    }
    af->image_sectorsize =sectorsize;
    if(af->badflag==0) af->badflag = (unsigned char *)malloc(sectorsize);
    else af->badflag = (unsigned char *)realloc(af->badflag,sectorsize);
    af->badflag_set = 0;

    if(af_update_seg(af,AF_SECTORSIZE,sectorsize,0,0)){
	if(errno != ENOTSUP) return -1;
    }
    return 0;
}