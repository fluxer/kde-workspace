
/*
Copyright 2010  Nicolas Ternisien <nicolas.ternisien@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "os_current.h"

#include "os_base.h"

#ifdef Q_OS_LINUX
	#include "info_linux.cpp"
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_DRAGONFLY)
	#include "info_fbsd.cpp"
#elif defined(Q_OS_NETBSD)
	#include "info_netbsd.cpp"
#elif defined(Q_OS_OPENBSD)
	#include "info_openbsd.cpp"
#elif defined(Q_OS_SOLARIS)
	#include "info_solaris.cpp"
#else
	#include "info_generic.cpp"	/* Default for unsupported systems.... */
#endif
