static SQInteger array_reverse(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_arrayreverse(v,-1)) ? 1 : SQ_ERROR;
}