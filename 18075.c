static SQInteger base_compilestring(HSQUIRRELVM v)
{
    SQInteger nargs=sq_gettop(v);
    const SQChar *src=NULL,*name=_SC("unnamedbuffer");
    SQInteger size;
    sq_getstring(v,2,&src);
    size=sq_getsize(v,2);
    if(nargs>2){
        sq_getstring(v,3,&name);
    }
    if(SQ_SUCCEEDED(sq_compilebuffer(v,src,size,name,SQFalse)))
        return 1;
    else
        return SQ_ERROR;
}