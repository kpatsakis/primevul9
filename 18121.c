static SQInteger base_getroottable(HSQUIRRELVM v)
{
    v->Push(v->_roottable);
    return 1;
}