static SQInteger _closure_acall(HSQUIRRELVM v,SQBool raiseerror)
{
    SQArray *aparams=_array(stack_get(v,2));
    SQInteger nparams=aparams->Size();
    v->Push(stack_get(v,1));
    for(SQInteger i=0;i<nparams;i++)v->Push(aparams->_values[i]);
    return SQ_SUCCEEDED(sq_call(v,nparams,SQTrue,raiseerror))?1:SQ_ERROR;
}