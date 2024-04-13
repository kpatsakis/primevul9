static SQInteger default_delegate_tostring(HSQUIRRELVM v)
{
    if(SQ_FAILED(sq_tostring(v,1)))
        return SQ_ERROR;
    return 1;
}