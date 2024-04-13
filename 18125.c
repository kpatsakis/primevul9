static SQInteger class_setattributes(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_setattributes(v,-3))?1:SQ_ERROR;
}