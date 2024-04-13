static void *conn_timeout_thread(void *arg) {
    int i;
    conn *c;
    char buf[TIMEOUT_MSG_SIZE];
    rel_time_t oldest_last_cmd;
    int sleep_time;
    useconds_t timeslice = 1000000 / (max_fds / CONNS_PER_SLICE);

    while(1) {
        if (settings.verbose > 2)
            fprintf(stderr, "idle timeout thread at top of connection list\n");

        oldest_last_cmd = current_time;

        for (i = 0; i < max_fds; i++) {
            if ((i % CONNS_PER_SLICE) == 0) {
                if (settings.verbose > 2)
                    fprintf(stderr, "idle timeout thread sleeping for %ulus\n",
                        timeslice);
                usleep(timeslice);
            }

            if (!conns[i])
                continue;

            c = conns[i];

            if (!IS_TCP(c->transport))
                continue;

            if (c->state != conn_new_cmd && c->state != conn_read)
                continue;

            if ((current_time - c->last_cmd_time) > settings.idle_timeout) {
                buf[0] = 't';
                memcpy(&buf[1], &i, sizeof(int));
                if (write(c->thread->notify_send_fd, buf, TIMEOUT_MSG_SIZE)
                    != TIMEOUT_MSG_SIZE)
                    perror("Failed to write timeout to notify pipe");
            } else {
                if (c->last_cmd_time < oldest_last_cmd)
                    oldest_last_cmd = c->last_cmd_time;
            }
        }

        /* This is the soonest we could have another connection time out */
        sleep_time = settings.idle_timeout - (current_time - oldest_last_cmd) + 1;
        if (sleep_time <= 0)
            sleep_time = 1;

        if (settings.verbose > 2)
            fprintf(stderr,
                    "idle timeout thread finished pass, sleeping for %ds\n",
                    sleep_time);
        usleep((useconds_t) sleep_time * 1000000);
    }

    return NULL;
}