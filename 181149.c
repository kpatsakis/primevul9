int ipc_msg_get_limits(struct ipc_limits *lim)
{
	if (access(_PATH_PROC_IPC_MSGMNI, F_OK) == 0 &&
	    access(_PATH_PROC_IPC_MSGMNB, F_OK) == 0 &&
	    access(_PATH_PROC_IPC_MSGMAX, F_OK) == 0) {

		if (ul_path_read_s32(NULL, &lim->msgmni, _PATH_PROC_IPC_MSGMNI) != 0)
			return 1;
		if (ul_path_read_s32(NULL, &lim->msgmnb, _PATH_PROC_IPC_MSGMNB) != 0)
			return 1;
		if (ul_path_read_u64(NULL, &lim->msgmax, _PATH_PROC_IPC_MSGMAX) != 0)
			return 1;
	} else {
		struct msginfo msginfo;

		if (msgctl(0, IPC_INFO, (struct msqid_ds *) &msginfo) < 0)
			return 1;
		lim->msgmni = msginfo.msgmni;
		lim->msgmnb = msginfo.msgmnb;
		lim->msgmax = msginfo.msgmax;
	}

	return 0;
}