#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT = .

include snek-install.defs

SNEK_OTHEROS?=1
ifeq ($(SNEK_OTHEROS),1)
SNEK_OTHEROS_DIR=linux windows
endif

SUBDIRS = posix snek-duino metro-snek snekde doc examples $(SNEK_OTHEROS_DIR)

all:
	+for dir in $(SUBDIRS); do (cd $$dir && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR)); done

%:
	+for dir in $(SUBDIRS); do (cd $$dir && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) $@); done

check: all
	+cd test && make $@

SHAREFILES = \
	snek.defs \
	$(SNEK_SRC) \
	$(SNEK_RAW_INC) \
	$(SNEK_BUILTINS) \
	$(SNEK_ROOT)/snek-gram.ll \
	$(SNEK_ROOT)/snek-builtin.py

IMAGEFILES = \
	snek.svg

DOCFILES = \
	$(IMAGEFILES)

PKGFILES = \
	snek.pc

install: $(SHAREFILES) $(PKGFILES) $(DOCFILES)
	install -d $(DESTDIR)$(SHAREDIR)
	for i in $(SHAREFILES); do install --mode=644 "$$i" $(DESTDIR)$(SHAREDIR); done
	install -d $(DESTDIR)$(PKGCONFIG)
	for i in $(PKGFILES); do install --mode=644 "$$i" $(DESTDIR)$(PKGCONFIG); done
	install -d $(DESTDIR)$(DOCDIR)
	for i in $(DOCFILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR); done
	+for dir in $(SUBDIRS); do (cd $$dir && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) install); done

upload:
	+cd doc && make upload
	+cd linux && make upload
	+cd windows && make upload

snek.pc: snek.pc.in
	$(SNEK_SED) $^ > $@
