static SQInteger string_slice(HSQUIRRELVM v)
{
    SQInteger sidx,eidx;
    SQObjectPtr o;
    if(SQ_FAILED(get_slice_params(v,sidx,eidx,o)))return -1;
    SQInteger slen = _string(o)->_len;
    if(sidx < 0)sidx = slen + sidx;
    if(eidx < 0)eidx = slen + eidx;
    if(eidx < sidx) return sq_throwerror(v,_SC("wrong indexes"));
    if(eidx > slen || sidx < 0) return sq_throwerror(v, _SC("slice out of range"));
    v->Push(SQString::Create(_ss(v),&_stringval(o)[sidx],eidx-sidx));
    return 1;
}