/*
 * Copyright (c) 1998 Christian Esken <esken@kde.org> 
 * Copyright (c) 2003 Oswald Buddenhagen <ossi@kde.org>
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
 *      Copyright (C) 1998, Christian Esken <esken@kde.org>
 */

#include "kcheckpass.h"

/*******************************************************************
 * This is the authentication code for /etc/passwd passwords
 *******************************************************************/

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>

AuthReturn Authenticate_etcpasswd(const char *login, const char *password)
{
  struct passwd *pw;
  char *crpt_passwd;

  openlog("kcheckpass", LOG_PID, LOG_AUTH);

  /* Get the password entry for the user we want */
  if (!(pw = getpwnam(login))) {
    syslog(LOG_ERR, "getpwnam: %s", strerror(errno));
    return AuthBad;
  }

  if (!*pw->pw_passwd)
    return AuthOk;

  if ((crpt_passwd = crypt(password, pw->pw_passwd)) && !strcmp(pw->pw_passwd, crpt_passwd)) {
    return AuthOk; /* Success */
  }
  return AuthBad; /* Password wrong or account locked */
}
