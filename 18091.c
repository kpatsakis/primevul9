static SQInteger default_delegate_len(HSQUIRRELVM v)
{
    v->Push(SQInteger(sq_getsize(v,1)));
    return 1;
}