static SQInteger table_getdelegate(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_getdelegate(v,-1))?1:SQ_ERROR;
}