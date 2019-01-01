# This file is part of cerebrum.

# cerebrum is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# cerebrum is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with cerebrum.  If not, see <https://www.gnu.org/licenses/>.

include $(top_builddir)/script/check.mk

bin_PROGRAMS= cerebrum-daemon

cerebrum_daemon_CFLAGS= \
	$(avahi_client_CFLAGS) \
	$(dbus_CFLAGS) \
	$(libcrypto_CFLAGS) \
	$(libdaemon_CFLAGS) \
	$(libevent_CFLAGS) \
	$(libevent_openssl_CFLAGS) \
	$(libssl_CFLAGS) \
	-I.

noinst_HEADERS= \
	daemon.h \
	daemon-alloc.h \
	daemon-cond.h \
	daemon-ctx.h \
	daemon-idle.h \
	daemon-list.h \
	daemon-loop.h \
	daemon-options.h \
	avahi/avahi-client.h \
	avahi/avahi-group.h \
	avahi/avahi-service.h \
	avahi/avahi-timer.h \
	avahi/avahi-watch.h \
	ssl/ssl.h \
	ssl/ssl-connection.h \
	ssl/ssl-server.h \
	ssl/ssl-packet.h

cerebrum_daemon_SOURCES= \
	daemon.c \
	daemon-client.c \
	daemon-ctx.c \
	daemon-group.c \
	daemon-idle.c \
	daemon-list.c \
	daemon-loop.c \
	daemon-options.c \
	daemon-main.c \
	daemon-ssl.c \
	avahi/avahi-client.c \
	avahi/avahi-group.c \
	avahi/avahi-loop.c \
	avahi/avahi-service.c \
	avahi/avahi-timer.c \
	avahi/avahi-watch.c \
	ssl/ssl-connection.c \
	ssl/ssl-server.c

cerebrum_daemon_LDFLAGS= \
	$(avahi_client_LIBS) \
	$(dbus_LIBS) \
	$(libcrypto_LIBS) \
	$(libdaemon_LIBS) \
	$(libevent_LIBS) \
	$(libevent_openssl_LIBS) \
	$(libssl_LIBS)

# eval to create the coding style rule
$(eval $(call check, $(sort $(noinst_HEADERS) $(cerebrum_daemon_SOURCES))))
