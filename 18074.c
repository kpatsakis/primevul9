static SQInteger container_rawget(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_rawget(v,-2))?1:SQ_ERROR;
}