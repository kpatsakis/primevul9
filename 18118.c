static SQInteger closure_getroot(HSQUIRRELVM v)
{
    if(SQ_FAILED(sq_getclosureroot(v,-1)))
        return SQ_ERROR;
    return 1;
}