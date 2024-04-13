static SQInteger closure_setroot(HSQUIRRELVM v)
{
    if(SQ_FAILED(sq_setclosureroot(v,-2)))
        return SQ_ERROR;
    return 1;
}