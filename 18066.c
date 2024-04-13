static SQInteger base_enabledebuginfo(HSQUIRRELVM v)
{
    SQObjectPtr &o=stack_get(v,2);

    sq_enabledebuginfo(v,SQVM::IsFalse(o)?SQFalse:SQTrue);
    return 0;
}