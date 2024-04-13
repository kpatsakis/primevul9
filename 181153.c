int ipc_sem_get_info(int id, struct sem_data **semds)
{
	FILE *f;
	int i = 0, maxid, j;
	struct sem_data *p;
	struct seminfo dummy;
	union semun arg;

	p = *semds = xcalloc(1, sizeof(struct sem_data));
	p->next = NULL;

	f = fopen(_PATH_PROC_SYSV_SEM, "r");
	if (!f)
		goto sem_fallback;

	while (fgetc(f) != '\n') ;	/* skip header */

	while (feof(f) == 0) {
		if (fscanf(f,
			   "%d %d  %o %" SCNu64 " %u %u %u %u %"
			    SCNi64 " %" SCNi64 "\n",
			   &p->sem_perm.key,
			   &p->sem_perm.id,
			   &p->sem_perm.mode,
			   &p->sem_nsems,
			   &p->sem_perm.uid,
			   &p->sem_perm.gid,
			   &p->sem_perm.cuid,
			   &p->sem_perm.cgid,
			   &p->sem_otime,
			   &p->sem_ctime) != 10)
			continue;

		if (id > -1) {
			/* ID specified */
			if (id == p->sem_perm.id) {
				get_sem_elements(p);
				i = 1;
				break;
			}
			continue;
		}

		p->next = xcalloc(1, sizeof(struct sem_data));
		p = p->next;
		p->next = NULL;
		i++;
	}

	if (i == 0)
		free(*semds);
	fclose(f);
	return i;

	/* Fallback; /proc or /sys file(s) missing. */
sem_fallback:
	arg.array = (ushort *) (void *)&dummy;
	maxid = semctl(0, 0, SEM_INFO, arg);

	for (j = 0; j <= maxid; j++) {
		int semid;
		struct semid_ds semseg;
		struct ipc_perm *ipcp = &semseg.sem_perm;
		arg.buf = (struct semid_ds *)&semseg;

		semid = semctl(j, 0, SEM_STAT, arg);
		if (semid < 0 || (id > -1 && semid != id)) {
			continue;
		}

		i++;
		p->sem_perm.key = ipcp->KEY;
		p->sem_perm.id = semid;
		p->sem_perm.mode = ipcp->mode;
		p->sem_nsems = semseg.sem_nsems;
		p->sem_perm.uid = ipcp->uid;
		p->sem_perm.gid = ipcp->gid;
		p->sem_perm.cuid = ipcp->cuid;
		p->sem_perm.cgid = ipcp->cuid;
		p->sem_otime = semseg.sem_otime;
		p->sem_ctime = semseg.sem_ctime;

		if (id < 0) {
			p->next = xcalloc(1, sizeof(struct sem_data));
			p = p->next;
			p->next = NULL;
			i++;
		} else {
			get_sem_elements(p);
			break;
		}
	}

	if (i == 0)
		free(*semds);
	return i;
}