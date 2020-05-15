/* Generated from ./ldap_config.hin on Fri May 15 10:53:12 CST 2020 */
/* $OpenLDAP$ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2020 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

/*
 * This file works in confunction with OpenLDAP configure system.
 * If you do no like the values below, adjust your configure options.
 */

#ifndef _LDAP_CONFIG_H
#define _LDAP_CONFIG_H

/* directory separator */
#ifndef LDAP_DIRSEP
#ifndef _WIN32
#define LDAP_DIRSEP "/"
#else
#define LDAP_DIRSEP "\\"
#endif
#endif

/* directory for temporary files */
#if defined(_WIN32)
# define LDAP_TMPDIR "C:\\."	/* we don't have much of a choice */
#elif defined( _P_tmpdir )
# define LDAP_TMPDIR _P_tmpdir
#elif defined( P_tmpdir )
# define LDAP_TMPDIR P_tmpdir
#elif defined( _PATH_TMPDIR )
# define LDAP_TMPDIR _PATH_TMPDIR
#else
# define LDAP_TMPDIR LDAP_DIRSEP "tmp"
#endif

/* directories */
#ifndef LDAP_BINDIR
#define LDAP_BINDIR			"/home/gorilla/test/bin"
#endif
#ifndef LDAP_SBINDIR
#define LDAP_SBINDIR		"/home/gorilla/test/sbin"
#endif
#ifndef LDAP_DATADIR
#define LDAP_DATADIR		"/home/gorilla/test/share/openldap"
#endif
#ifndef LDAP_SYSCONFDIR
#define LDAP_SYSCONFDIR		"/home/gorilla/test/etc/openldap"
#endif
#ifndef LDAP_LIBEXECDIR
#define LDAP_LIBEXECDIR		"/home/gorilla/test/libexec"
#endif
#ifndef LDAP_MODULEDIR
#define LDAP_MODULEDIR		"/home/gorilla/test/libexec/openldap"
#endif
#ifndef LDAP_RUNDIR
#define LDAP_RUNDIR			"/home/gorilla/test/var"
#endif
#ifndef LDAP_LOCALEDIR
#define LDAP_LOCALEDIR		""
#endif


#endif /* _LDAP_CONFIG_H */
