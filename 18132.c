static SQInteger obj_delegate_weakref(HSQUIRRELVM v)
{
    sq_weakref(v,1);
    return 1;
}