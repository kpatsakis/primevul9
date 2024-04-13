static SQInteger class_getattributes(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_getattributes(v,-2))?1:SQ_ERROR;
}