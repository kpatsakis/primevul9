qmp_guest_fstrim(bool has_minimum, int64_t minimum, Error **errp)
{
    GuestFilesystemTrimResponse *resp;
    HANDLE handle;
    WCHAR guid[MAX_PATH] = L"";

    handle = FindFirstVolumeW(guid, ARRAYSIZE(guid));
    if (handle == INVALID_HANDLE_VALUE) {
        error_setg_win32(errp, GetLastError(), "failed to find any volume");
        return NULL;
    }

    resp = g_new0(GuestFilesystemTrimResponse, 1);

    do {
        GuestFilesystemTrimResult *res;
        GuestFilesystemTrimResultList *list;
        PWCHAR uc_path;
        DWORD char_count = 0;
        char *path, *out;
        GError *gerr = NULL;
        gchar * argv[4];

        GetVolumePathNamesForVolumeNameW(guid, NULL, 0, &char_count);

        if (GetLastError() != ERROR_MORE_DATA) {
            continue;
        }
        if (GetDriveTypeW(guid) != DRIVE_FIXED) {
            continue;
        }

        uc_path = g_malloc(sizeof(WCHAR) * char_count);
        if (!GetVolumePathNamesForVolumeNameW(guid, uc_path, char_count,
                                              &char_count) || !*uc_path) {
            /* strange, but this condition could be faced even with size == 2 */
            g_free(uc_path);
            continue;
        }

        res = g_new0(GuestFilesystemTrimResult, 1);

        path = g_utf16_to_utf8(uc_path, char_count, NULL, NULL, &gerr);

        g_free(uc_path);

        if (!path) {
            res->has_error = true;
            res->error = g_strdup(gerr->message);
            g_error_free(gerr);
            break;
        }

        res->path = path;

        list = g_new0(GuestFilesystemTrimResultList, 1);
        list->value = res;
        list->next = resp->paths;

        resp->paths = list;

        memset(argv, 0, sizeof(argv));
        argv[0] = (gchar *)"defrag.exe";
        argv[1] = (gchar *)"/L";
        argv[2] = path;

        if (!g_spawn_sync(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
                          &out /* stdout */, NULL /* stdin */,
                          NULL, &gerr)) {
            res->has_error = true;
            res->error = g_strdup(gerr->message);
            g_error_free(gerr);
        } else {
            /* defrag.exe is UGLY. Exit code is ALWAYS zero.
               Error is reported in the output with something like
               (x89000020) etc code in the stdout */

            int i;
            gchar **lines = g_strsplit(out, "\r\n", 0);
            g_free(out);

            for (i = 0; lines[i] != NULL; i++) {
                if (g_strstr_len(lines[i], -1, "(0x") == NULL) {
                    continue;
                }
                res->has_error = true;
                res->error = g_strdup(lines[i]);
                break;
            }
            g_strfreev(lines);
        }
    } while (FindNextVolumeW(handle, guid, ARRAYSIZE(guid)));

    FindVolumeClose(handle);
    return resp;
}