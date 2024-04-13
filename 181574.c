guestfs___check_package_management (guestfs_h *g, struct inspect_fs *fs)
{
  switch (fs->distro) {
  case OS_DISTRO_FEDORA:
  case OS_DISTRO_MEEGO:
    fs->package_management = OS_PACKAGE_MANAGEMENT_YUM;
    break;

  case OS_DISTRO_REDHAT_BASED:
  case OS_DISTRO_RHEL:
  case OS_DISTRO_CENTOS:
  case OS_DISTRO_SCIENTIFIC_LINUX:
    if (fs->major_version >= 5)
      fs->package_management = OS_PACKAGE_MANAGEMENT_YUM;
    else
      fs->package_management = OS_PACKAGE_MANAGEMENT_UP2DATE;
    break;

  case OS_DISTRO_DEBIAN:
  case OS_DISTRO_UBUNTU:
  case OS_DISTRO_LINUX_MINT:
    fs->package_management = OS_PACKAGE_MANAGEMENT_APT;
    break;

  case OS_DISTRO_ARCHLINUX:
    fs->package_management = OS_PACKAGE_MANAGEMENT_PACMAN;
    break;
  case OS_DISTRO_GENTOO:
    fs->package_management = OS_PACKAGE_MANAGEMENT_PORTAGE;
    break;
  case OS_DISTRO_PARDUS:
    fs->package_management = OS_PACKAGE_MANAGEMENT_PISI;
    break;
  case OS_DISTRO_MAGEIA:
  case OS_DISTRO_MANDRIVA:
    fs->package_management = OS_PACKAGE_MANAGEMENT_URPMI;
    break;

  case OS_DISTRO_SUSE_BASED:
  case OS_DISTRO_OPENSUSE:
  case OS_DISTRO_SLES:
    fs->package_management = OS_PACKAGE_MANAGEMENT_ZYPPER;
    break;

  case OS_DISTRO_SLACKWARE:
  case OS_DISTRO_TTYLINUX:
  case OS_DISTRO_WINDOWS:
  case OS_DISTRO_BUILDROOT:
  case OS_DISTRO_CIRROS:
  case OS_DISTRO_FREEDOS:
  case OS_DISTRO_OPENBSD:
  case OS_DISTRO_UNKNOWN:
    fs->package_management = OS_PACKAGE_MANAGEMENT_UNKNOWN;
    break;
  }
}