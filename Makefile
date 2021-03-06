DIRS=	util loader installer
.EXPORT_ALL_VARIABLES:

all clean dep depend:
	@for I in $(DIRS); do cd $$I; make $@ || exit 1; cd ..; done

install:
	install -d -m 755 $(DESTDIR)/sbin
	install -d -m 755 $(DESTDIR)/boot
	install -d -m 755 $(DESTDIR)/etc
	install -d -m 755 $(DESTDIR)/usr/man/man5
	install -d -m 755 $(DESTDIR)/usr/man/man8
	install -s installer/iquik $(DESTDIR)/sbin
	install -m 444 loader/iquik.b $(DESTDIR)/boot
	if [ -f $(DESTDIR)/etc/quik.conf ]; then :; \
	  else set -x; install -m 644 etc/quik.conf $(DESTDIR)/etc; fi
	install -m 444 man/quik.conf.5 $(DESTDIR)/usr/man/man5
	install -m 444 man/quik.8 man/bootstrap.8 $(DESTDIR)/usr/man/man8
