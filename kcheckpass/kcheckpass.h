/*****************************************************************
 *
 *	kcheckpass
 *
 *	Simple password checker. Just invoke and send it
 *	the password on stdin.
 *
 *	If the password was accepted, the program exits with 0;
 *	if it was rejected, it exits with 1. Any other exit
 *	code signals an error.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *	Copyright (C) 1998, Caldera, Inc.
 *	Released under the GNU General Public License
 *
 *	Olaf Kirch <okir@caldera.de>      General Framework and PAM support
 *	Christian Esken <esken@kde.org>   Shadow and /etc/passwd support
 *	Oswald Buddenhagen <ossi@kde.org> Binary server mode
 *
 *      Other parts were taken from kscreensaver's passwd.cpp
 *****************************************************************/

#ifndef KCHECKPASS_H_
#define KCHECKPASS_H_

#include <config-workspace.h>
#include <config-unix.h>

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_GETSPNAM
# define HAVE_SHADOW
#endif

#if !defined(__INSURE__)
# define ATTR_UNUSED __attribute__((unused))
# define ATTR_NORETURN __attribute__((noreturn))
# define ATTR_PRINTFLIKE(fmt,var) __attribute__((format(printf,fmt,var)))
#else
# define ATTR_UNUSED
# define ATTR_NORETURN
# define ATTR_PRINTFLIKE(fmt,var)
#endif

#include "kcheckpass-enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * Authenticates user
 *****************************************************************/
#ifdef HAVE_PAM
AuthReturn Authenticate_pam(
    const char *caller,
    const char *user,
    const char *password,
    char *(*conv) (ConvRequest, const char *)
);
#endif

#ifdef HAVE_SHADOW
AuthReturn Authenticate_shadow(
    const char *user,
    const char *password
);
#endif

AuthReturn Authenticate_etcpasswd(
    const char *user,
    const char *password
);

/*****************************************************************
 * Output a message to stderr
 *****************************************************************/
void message(const char *, ...) ATTR_PRINTFLIKE(1, 2);

/*****************************************************************
 * Overwrite and free the passed string
 *****************************************************************/
void dispose(char *);

#ifdef __cplusplus
}
#endif
#endif
