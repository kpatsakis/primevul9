add_ld_so_conf (FlatpakBwrap *bwrap,
                GError      **error)
{
  const char *contents =
    "include /run/flatpak/ld.so.conf.d/app-*.conf\n"
    "include /app/etc/ld.so.conf\n"
    "/app/lib\n"
    "include /run/flatpak/ld.so.conf.d/runtime-*.conf\n";

  return flatpak_bwrap_add_args_data (bwrap, "ld-so-conf",
                                      contents, -1, "/etc/ld.so.conf", error);
}