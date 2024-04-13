static SQInteger obj_clear(HSQUIRRELVM v)
{
    return SQ_SUCCEEDED(sq_clear(v,-1)) ? 1 : SQ_ERROR;
}