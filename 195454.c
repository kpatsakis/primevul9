int manager_deserialize(Manager *m, FILE *f, FDSet *fds) {
        int r = 0;

        assert(m);
        assert(f);

        log_debug("Deserializing state...");

        m->n_reloading ++;

        for (;;) {
                char line[LINE_MAX], *l;

                if (!fgets(line, sizeof(line), f)) {
                        if (feof(f))
                                r = 0;
                        else
                                r = -errno;

                        goto finish;
                }

                char_array_0(line);
                l = strstrip(line);

                if (l[0] == 0)
                        break;

                if (startswith(l, "current-job-id=")) {
                        uint32_t id;

                        if (safe_atou32(l+15, &id) < 0)
                                log_debug("Failed to parse current job id value %s", l+15);
                        else
                                m->current_job_id = MAX(m->current_job_id, id);

                } else if (startswith(l, "n-installed-jobs=")) {
                        uint32_t n;

                        if (safe_atou32(l+17, &n) < 0)
                                log_debug("Failed to parse installed jobs counter %s", l+17);
                        else
                                m->n_installed_jobs += n;

                } else if (startswith(l, "n-failed-jobs=")) {
                        uint32_t n;

                        if (safe_atou32(l+14, &n) < 0)
                                log_debug("Failed to parse failed jobs counter %s", l+14);
                        else
                                m->n_failed_jobs += n;

                } else if (startswith(l, "taint-usr=")) {
                        int b;

                        b = parse_boolean(l+10);
                        if (b < 0)
                                log_debug("Failed to parse taint /usr flag %s", l+10);
                        else
                                m->taint_usr = m->taint_usr || b;

                } else if (startswith(l, "firmware-timestamp="))
                        dual_timestamp_deserialize(l+19, &m->firmware_timestamp);
                else if (startswith(l, "loader-timestamp="))
                        dual_timestamp_deserialize(l+17, &m->loader_timestamp);
                else if (startswith(l, "kernel-timestamp="))
                        dual_timestamp_deserialize(l+17, &m->kernel_timestamp);
                else if (startswith(l, "initrd-timestamp="))
                        dual_timestamp_deserialize(l+17, &m->initrd_timestamp);
                else if (startswith(l, "userspace-timestamp="))
                        dual_timestamp_deserialize(l+20, &m->userspace_timestamp);
                else if (startswith(l, "finish-timestamp="))
                        dual_timestamp_deserialize(l+17, &m->finish_timestamp);
                else if (startswith(l, "security-start-timestamp="))
                        dual_timestamp_deserialize(l+25, &m->security_start_timestamp);
                else if (startswith(l, "security-finish-timestamp="))
                        dual_timestamp_deserialize(l+26, &m->security_finish_timestamp);
                else if (startswith(l, "generators-start-timestamp="))
                        dual_timestamp_deserialize(l+27, &m->generators_start_timestamp);
                else if (startswith(l, "generators-finish-timestamp="))
                        dual_timestamp_deserialize(l+28, &m->generators_finish_timestamp);
                else if (startswith(l, "units-load-start-timestamp="))
                        dual_timestamp_deserialize(l+27, &m->units_load_start_timestamp);
                else if (startswith(l, "units-load-finish-timestamp="))
                        dual_timestamp_deserialize(l+28, &m->units_load_finish_timestamp);
                else if (startswith(l, "env=")) {
                        _cleanup_free_ char *uce = NULL;
                        char **e;

                        uce = cunescape(l+4);
                        if (!uce) {
                                r = -ENOMEM;
                                goto finish;
                        }

                        e = strv_env_set(m->environment, uce);
                        if (!e) {
                                r = -ENOMEM;
                                goto finish;
                        }

                        strv_free(m->environment);
                        m->environment = e;

                } else if (startswith(l, "notify-fd=")) {
                        int fd;

                        if (safe_atoi(l + 10, &fd) < 0 || fd < 0 || !fdset_contains(fds, fd))
                                log_debug("Failed to parse notify fd: %s", l + 10);
                        else {
                                m->notify_event_source = sd_event_source_unref(m->notify_event_source);
                                safe_close(m->notify_fd);
                                m->notify_fd = fdset_remove(fds, fd);
                        }

                } else if (startswith(l, "notify-socket=")) {
                        char *n;

                        n = strdup(l+14);
                        if (!n) {
                                r = -ENOMEM;
                                goto finish;
                        }

                        free(m->notify_socket);
                        m->notify_socket = n;

                } else if (startswith(l, "kdbus-fd=")) {
                        int fd;

                        if (safe_atoi(l + 9, &fd) < 0 || fd < 0 || !fdset_contains(fds, fd))
                                log_debug("Failed to parse kdbus fd: %s", l + 9);
                        else {
                                safe_close(m->kdbus_fd);
                                m->kdbus_fd = fdset_remove(fds, fd);
                        }

                } else {
                        int k;

                        k = bus_track_deserialize_item(&m->deserialized_subscribed, l);
                        if (k < 0)
                                log_debug_errno(k, "Failed to deserialize bus tracker object: %m");
                        else if (k == 0)
                                log_debug("Unknown serialization item '%s'", l);
                }
        }

        for (;;) {
                Unit *u;
                char name[UNIT_NAME_MAX+2];

                /* Start marker */
                if (!fgets(name, sizeof(name), f)) {
                        if (feof(f))
                                r = 0;
                        else
                                r = -errno;

                        goto finish;
                }

                char_array_0(name);

                r = manager_load_unit(m, strstrip(name), NULL, NULL, &u);
                if (r < 0)
                        goto finish;

                r = unit_deserialize(u, f, fds);
                if (r < 0)
                        goto finish;
        }

finish:
        if (ferror(f))
                r = -EIO;

        assert(m->n_reloading > 0);
        m->n_reloading --;

        return r;
}