static SQInteger array_append(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_arrayappend(v,-2)) ? 1 : SQ_ERROR;
}