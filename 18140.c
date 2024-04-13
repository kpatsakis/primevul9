static SQInteger array_find(HSQUIRRELVM v)
{
    SQObject &o = stack_get(v,1);
    SQObjectPtr &val = stack_get(v,2);
    SQArray *a = _array(o);
    SQInteger size = a->Size();
    SQObjectPtr temp;
    for(SQInteger n = 0; n < size; n++) {
        bool res = false;
        a->Get(n,temp);
        if(SQVM::IsEqual(temp,val,res) && res) {
            v->Push(n);
            return 1;
        }
    }
    return 0;
}