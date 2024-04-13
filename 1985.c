open_namespace_fd_if_needed (const char *path,
                             const char *other_path) {
  struct stat s, other_s;

  if (stat (path, &s) != 0)
    return -1; /* No such namespace, ignore */

  if (stat (other_path, &other_s) != 0)
    return -1; /* No such namespace, ignore */

  /* setns calls fail if the process is already in the desired namespace, hence the
     check here to ensure the namespaces are different. */
  if (s.st_ino != other_s.st_ino)
    return open (path, O_RDONLY|O_CLOEXEC);

  return -1;
}