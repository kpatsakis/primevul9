static SQInteger generator_getstatus(HSQUIRRELVM v)
{
    SQObject &o=stack_get(v,1);
    switch(_generator(o)->_state){
        case SQGenerator::eSuspended:v->Push(SQString::Create(_ss(v),_SC("suspended")));break;
        case SQGenerator::eRunning:v->Push(SQString::Create(_ss(v),_SC("running")));break;
        case SQGenerator::eDead:v->Push(SQString::Create(_ss(v),_SC("dead")));break;
    }
    return 1;
}