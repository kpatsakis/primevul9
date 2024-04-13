static SQInteger array_pop(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_arraypop(v,1,SQTrue))?1:SQ_ERROR;
}