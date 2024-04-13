static SQInteger array_filter(HSQUIRRELVM v)
{
    SQObject &o = stack_get(v,1);
    SQArray *a = _array(o);
    SQObjectPtr ret = SQArray::Create(_ss(v),0);
    SQInteger size = a->Size();
    SQObjectPtr val;
    for(SQInteger n = 0; n < size; n++) {
        a->Get(n,val);
        v->Push(o);
        v->Push(n);
        v->Push(val);
        if(SQ_FAILED(sq_call(v,3,SQTrue,SQFalse))) {
            return SQ_ERROR;
        }
        if(!SQVM::IsFalse(v->GetUp(-1))) {
            _array(ret)->Append(val);
        }
        v->Pop();
    }
    v->Push(ret);
    return 1;
}