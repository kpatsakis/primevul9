static SQInteger class_getbase(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_getbase(v,-1))?1:SQ_ERROR;
}