static SQInteger string_find(HSQUIRRELVM v)
{
    SQInteger top,start_idx=0;
    const SQChar *str,*substr,*ret;
    if(((top=sq_gettop(v))>1) && SQ_SUCCEEDED(sq_getstring(v,1,&str)) && SQ_SUCCEEDED(sq_getstring(v,2,&substr))){
        if(top>2)sq_getinteger(v,3,&start_idx);
        if((sq_getsize(v,1)>start_idx) && (start_idx>=0)){
            ret=scstrstr(&str[start_idx],substr);
            if(ret){
                sq_pushinteger(v,(SQInteger)(ret-str));
                return 1;
            }
        }
        return 0;
    }
    return sq_throwerror(v,_SC("invalid param"));
}