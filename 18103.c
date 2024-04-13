static SQInteger container_rawexists(HSQUIRRELVM v)
{
    if(SQ_SUCCEEDED(sq_rawget(v,-2))) {
        sq_pushbool(v,SQTrue);
        return 1;
    }
    sq_pushbool(v,SQFalse);
    return 1;
}