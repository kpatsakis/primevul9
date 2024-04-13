SYSCALL_DEFINE6(sparc_ipc, unsigned int, call, int, first, unsigned long, second,
		unsigned long, third, void __user *, ptr, long, fifth)
{
	long err;

	/* No need for backward compatibility. We can start fresh... */
	if (call <= SEMCTL) {
		switch (call) {
		case SEMOP:
			err = sys_semtimedop(first, ptr,
					     (unsigned)second, NULL);
			goto out;
		case SEMTIMEDOP:
			err = sys_semtimedop(first, ptr, (unsigned)second,
				(const struct timespec __user *)
					     (unsigned long) fifth);
			goto out;
		case SEMGET:
			err = sys_semget(first, (int)second, (int)third);
			goto out;
		case SEMCTL: {
			err = sys_semctl(first, second,
					 (int)third | IPC_64,
					 (union semun) ptr);
			goto out;
		}
		default:
			err = -ENOSYS;
			goto out;
		};
	}
	if (call <= MSGCTL) {
		switch (call) {
		case MSGSND:
			err = sys_msgsnd(first, ptr, (size_t)second,
					 (int)third);
			goto out;
		case MSGRCV:
			err = sys_msgrcv(first, ptr, (size_t)second, fifth,
					 (int)third);
			goto out;
		case MSGGET:
			err = sys_msgget((key_t)first, (int)second);
			goto out;
		case MSGCTL:
			err = sys_msgctl(first, (int)second | IPC_64, ptr);
			goto out;
		default:
			err = -ENOSYS;
			goto out;
		};
	}
	if (call <= SHMCTL) {
		switch (call) {
		case SHMAT: {
			ulong raddr;
			err = do_shmat(first, ptr, (int)second, &raddr);
			if (!err) {
				if (put_user(raddr,
					     (ulong __user *) third))
					err = -EFAULT;
			}
			goto out;
		}
		case SHMDT:
			err = sys_shmdt(ptr);
			goto out;
		case SHMGET:
			err = sys_shmget(first, (size_t)second, (int)third);
			goto out;
		case SHMCTL:
			err = sys_shmctl(first, (int)second | IPC_64, ptr);
			goto out;
		default:
			err = -ENOSYS;
			goto out;
		};
	} else {
		err = -ENOSYS;
	}
out:
	return err;
}