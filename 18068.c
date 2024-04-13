static SQInteger base_getstackinfos(HSQUIRRELVM v)
{
    SQInteger level;
    sq_getinteger(v, -1, &level);
    return __getcallstackinfos(v,level);
}