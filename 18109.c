static SQInteger container_rawset(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_rawset(v,-3)) ? 1 : SQ_ERROR;
}