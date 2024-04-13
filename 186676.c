static void mptsas_process_message(MPTSASState *s, MPIRequestHeader *req)
{
    trace_mptsas_process_message(s, req->Function, req->MsgContext);
    switch (req->Function) {
    case MPI_FUNCTION_SCSI_TASK_MGMT:
        mptsas_process_scsi_task_mgmt(s, (MPIMsgSCSITaskMgmt *)req);
        break;

    case MPI_FUNCTION_IOC_INIT:
        mptsas_process_ioc_init(s, (MPIMsgIOCInit *)req);
        break;

    case MPI_FUNCTION_IOC_FACTS:
        mptsas_process_ioc_facts(s, (MPIMsgIOCFacts *)req);
        break;

    case MPI_FUNCTION_PORT_FACTS:
        mptsas_process_port_facts(s, (MPIMsgPortFacts *)req);
        break;

    case MPI_FUNCTION_PORT_ENABLE:
        mptsas_process_port_enable(s, (MPIMsgPortEnable *)req);
        break;

    case MPI_FUNCTION_EVENT_NOTIFICATION:
        mptsas_process_event_notification(s, (MPIMsgEventNotify *)req);
        break;

    case MPI_FUNCTION_CONFIG:
        mptsas_process_config(s, (MPIMsgConfig *)req);
        break;

    default:
        trace_mptsas_unhandled_cmd(s, req->Function, 0);
        mptsas_set_fault(s, MPI_IOCSTATUS_INVALID_FUNCTION);
        break;
    }
}