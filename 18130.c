static SQInteger class_instance(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_createinstance(v,-1))?1:SQ_ERROR;
}