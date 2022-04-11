/*
 * Copyright (C) 1998 Christian Esken <esken@kde.org>
 * Copyright (C) 2003 Oswald Buddenhagen <ossi@kde.org>
 *
 * This is a modified version of checkpass_shadow.cpp
 *
 * Modifications made by Thorsten Kukuk <kukuk@suse.de>
 *                       Mathias Kettner <kettner@suse.de>
 *
 * ------------------------------------------------------------
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
 */

#include "kcheckpass.h"

/*******************************************************************
 * This is the authentication code for Shadow-Passwords
 *******************************************************************/

#ifdef HAVE_SHADOW

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <pwd.h>
#include <shadow.h>
#include <unistd.h>
#include <sys/types.h>

AuthReturn Authenticate_shadow(const char *login, const char *typed_in_password)
{
  char          *crpt_passwd;
  char          *password;
  struct passwd *pw;
  struct spwd   *spw;

  openlog("kcheckpass", LOG_PID, LOG_AUTH);

  if (!(pw = getpwnam(login))) {
    syslog(LOG_ERR, "getpwnam: %s", strerror(errno));
    return AuthAbort;
  }

  uid_t eid = geteuid();
  if (eid != 0 && seteuid(0) != 0) {
    syslog(LOG_ERR, "seteuid: %s", strerror(errno));
    return AuthAbort;
  }

  spw = getspnam(login);
  password = spw ? spw->sp_pwdp : pw->pw_passwd;

  if (!*password) {
    seteuid(eid);
    return AuthOk;
  }

#if defined( __linux__ ) && defined( HAVE_PW_ENCRYPT )
  crpt_passwd = pw_encrypt(typed_in_password, password);  /* (1) */
#else  
  crpt_passwd = crypt(typed_in_password, password);
#endif

  if (crpt_passwd && !strcmp(password, crpt_passwd )) {
    seteuid(eid);
    return AuthOk; /* Success */
  }
  seteuid(eid);
  return AuthBad; /* Password wrong or account locked */
}

/*
 (1) Deprecated - long passwords have known weaknesses.  Also,
     pw_encrypt is non-standard (requires libshadow.a) while
     everything else you need to support shadow passwords is in
     the standard (ELF) libc.
 */
#endif // HAVE_SHADOW
