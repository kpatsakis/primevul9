int ipc_msg_get_info(int id, struct msg_data **msgds)
{
	FILE *f;
	int i = 0, maxid, j;
	struct msg_data *p;
	struct msqid_ds dummy;
	struct msqid_ds msgseg;

	p = *msgds = xcalloc(1, sizeof(struct msg_data));
	p->next = NULL;

	f = fopen(_PATH_PROC_SYSV_MSG, "r");
	if (!f)
		goto msg_fallback;

	while (fgetc(f) != '\n') ;	/* skip header */

	while (feof(f) == 0) {
		if (fscanf(f,
			   "%d %d  %o  %" SCNu64 " %" SCNu64
			   " %u %u %u %u %u %u %" SCNi64 " %" SCNi64 " %" SCNi64 "\n",
			   &p->msg_perm.key,
			   &p->msg_perm.id,
			   &p->msg_perm.mode,
			   &p->q_cbytes,
			   &p->q_qnum,
			   &p->q_lspid,
			   &p->q_lrpid,
			   &p->msg_perm.uid,
			   &p->msg_perm.gid,
			   &p->msg_perm.cuid,
			   &p->msg_perm.cgid,
			   &p->q_stime,
			   &p->q_rtime,
			   &p->q_ctime) != 14)
			continue;

		if (id > -1) {
			/* ID specified */
			if (id == p->msg_perm.id) {
				if (msgctl(id, IPC_STAT, &msgseg) != -1)
					p->q_qbytes = msgseg.msg_qbytes;
				i = 1;
				break;
			}
			continue;
		}

		p->next = xcalloc(1, sizeof(struct msg_data));
		p = p->next;
		p->next = NULL;
		i++;
	}

	if (i == 0)
		free(*msgds);
	fclose(f);
	return i;

	/* Fallback; /proc or /sys file(s) missing. */
msg_fallback:
	maxid = msgctl(0, MSG_INFO, &dummy);

	for (j = 0; j <= maxid; j++) {
		int msgid;
		struct ipc_perm *ipcp = &msgseg.msg_perm;

		msgid = msgctl(j, MSG_STAT, &msgseg);
		if (msgid < 0 || (id > -1 && msgid != id)) {
			continue;
		}

		i++;
		p->msg_perm.key = ipcp->KEY;
		p->msg_perm.id = msgid;
		p->msg_perm.mode = ipcp->mode;
		p->q_cbytes = msgseg.msg_cbytes;
		p->q_qnum = msgseg.msg_qnum;
		p->q_lspid = msgseg.msg_lspid;
		p->q_lrpid = msgseg.msg_lrpid;
		p->msg_perm.uid = ipcp->uid;
		p->msg_perm.gid = ipcp->gid;
		p->msg_perm.cuid = ipcp->cuid;
		p->msg_perm.cgid = ipcp->cgid;
		p->q_stime = msgseg.msg_stime;
		p->q_rtime = msgseg.msg_rtime;
		p->q_ctime = msgseg.msg_ctime;
		p->q_qbytes = msgseg.msg_qbytes;

		if (id < 0) {
			p->next = xcalloc(1, sizeof(struct msg_data));
			p = p->next;
			p->next = NULL;
		} else
			break;
	}

	if (i == 0)
		free(*msgds);
	return i;
}