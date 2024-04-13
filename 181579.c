guestfs___check_package_format (guestfs_h *g, struct inspect_fs *fs)
{
  switch (fs->distro) {
  case OS_DISTRO_FEDORA:
  case OS_DISTRO_MEEGO:
  case OS_DISTRO_REDHAT_BASED:
  case OS_DISTRO_RHEL:
  case OS_DISTRO_MAGEIA:
  case OS_DISTRO_MANDRIVA:
  case OS_DISTRO_SUSE_BASED:
  case OS_DISTRO_OPENSUSE:
  case OS_DISTRO_SLES:
  case OS_DISTRO_CENTOS:
  case OS_DISTRO_SCIENTIFIC_LINUX:
    fs->package_format = OS_PACKAGE_FORMAT_RPM;
    break;

  case OS_DISTRO_DEBIAN:
  case OS_DISTRO_UBUNTU:
  case OS_DISTRO_LINUX_MINT:
    fs->package_format = OS_PACKAGE_FORMAT_DEB;
    break;

  case OS_DISTRO_ARCHLINUX:
    fs->package_format = OS_PACKAGE_FORMAT_PACMAN;
    break;
  case OS_DISTRO_GENTOO:
    fs->package_format = OS_PACKAGE_FORMAT_EBUILD;
    break;
  case OS_DISTRO_PARDUS:
    fs->package_format = OS_PACKAGE_FORMAT_PISI;
    break;

  case OS_DISTRO_SLACKWARE:
  case OS_DISTRO_TTYLINUX:
  case OS_DISTRO_WINDOWS:
  case OS_DISTRO_BUILDROOT:
  case OS_DISTRO_CIRROS:
  case OS_DISTRO_FREEDOS:
  case OS_DISTRO_OPENBSD:
  case OS_DISTRO_UNKNOWN:
    fs->package_format = OS_PACKAGE_FORMAT_UNKNOWN;
    break;
  }
}