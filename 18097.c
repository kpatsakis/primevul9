static SQInteger base_collectgarbage(HSQUIRRELVM v)
{
    sq_pushinteger(v, sq_collectgarbage(v));
    return 1;
}