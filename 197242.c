cdrom_setspeed(const char *spec) {
#define CDROM_SELECT_SPEED      0x5322  /* Set the CD-ROM speed */
	if (opt_speed) {
		int cdrom;
		int speed = atoi(opt_speed);

		if ((cdrom = open(spec, O_RDONLY | O_NONBLOCK)) < 0)
			die(EX_FAIL,
			    _("mount: cannot open %s for setting speed"),
			    spec);
		if (ioctl(cdrom, CDROM_SELECT_SPEED, speed) < 0)
			die(EX_FAIL, _("mount: cannot set speed: %m"));
		close(cdrom);
	}
}