static SQInteger base_getconsttable(HSQUIRRELVM v)
{
    v->Push(_ss(v)->_consts);
    return 1;
}