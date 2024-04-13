static SQInteger base_print(HSQUIRRELVM v)
{
    const SQChar *str;
    if(SQ_SUCCEEDED(sq_tostring(v,2)))
    {
        if(SQ_SUCCEEDED(sq_getstring(v,-1,&str))) {
            if(_ss(v)->_printfunc) _ss(v)->_printfunc(v,_SC("%s"),str);
            return 0;
        }
    }
    return SQ_ERROR;
}