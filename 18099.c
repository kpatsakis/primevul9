static SQInteger array_slice(HSQUIRRELVM v)
{
    SQInteger sidx,eidx;
    SQObjectPtr o;
    if(get_slice_params(v,sidx,eidx,o)==-1)return -1;
    SQInteger alen = _array(o)->Size();
    if(sidx < 0)sidx = alen + sidx;
    if(eidx < 0)eidx = alen + eidx;
    if(eidx < sidx)return sq_throwerror(v,_SC("wrong indexes"));
    if(eidx > alen || sidx < 0)return sq_throwerror(v, _SC("slice out of range"));
    SQArray *arr=SQArray::Create(_ss(v),eidx-sidx);
    SQObjectPtr t;
    SQInteger count=0;
    for(SQInteger i=sidx;i<eidx;i++){
        _array(o)->Get(i,t);
        arr->Set(count++,t);
    }
    v->Push(arr);
    return 1;

}