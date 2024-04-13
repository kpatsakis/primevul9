static SQInteger array_reduce(HSQUIRRELVM v)
{
    SQObject &o = stack_get(v,1);
    SQArray *a = _array(o);
    SQInteger size = a->Size();
    SQObjectPtr res;
    SQInteger iterStart;
    if (sq_gettop(v)>2) {
        res = stack_get(v,3);
        iterStart = 0;
    } else if (size==0) {
        return 0;
    } else {
        a->Get(0,res);
        iterStart = 1;
    }
    if (size > iterStart) {
        SQObjectPtr other;
        v->Push(stack_get(v,2));
        for (SQInteger n = iterStart; n < size; n++) {
            a->Get(n,other);
            v->Push(o);
            v->Push(res);
            v->Push(other);
            if(SQ_FAILED(sq_call(v,3,SQTrue,SQFalse))) {
                return SQ_ERROR;
            }
            res = v->GetUp(-1);
            v->Pop();
        }
        v->Pop();
    }
    v->Push(res);
    return 1;
}