static SQInteger thread_wakeup(HSQUIRRELVM v)
{
    SQObjectPtr o = stack_get(v,1);
    if(sq_type(o) == OT_THREAD) {
        SQVM *thread = _thread(o);
        SQInteger state = sq_getvmstate(thread);
        if(state != SQ_VMSTATE_SUSPENDED) {
            switch(state) {
                case SQ_VMSTATE_IDLE:
                    return sq_throwerror(v,_SC("cannot wakeup a idle thread"));
                break;
                case SQ_VMSTATE_RUNNING:
                    return sq_throwerror(v,_SC("cannot wakeup a running thread"));
                break;
            }
        }

        SQInteger wakeupret = sq_gettop(v)>1?SQTrue:SQFalse;
        if(wakeupret) {
            sq_move(thread,v,2);
        }
        if(SQ_SUCCEEDED(sq_wakeupvm(thread,wakeupret,SQTrue,SQTrue,SQFalse))) {
            sq_move(v,thread,-1);
            sq_pop(thread,1); //pop retval
            if(sq_getvmstate(thread) == SQ_VMSTATE_IDLE) {
                sq_settop(thread,1); //pop roottable
            }
            return 1;
        }
        sq_settop(thread,1);
        v->_lasterror = thread->_lasterror;
        return SQ_ERROR;
    }
    return sq_throwerror(v,_SC("wrong parameter"));
}