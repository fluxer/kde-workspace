/*
 *  helper.h
 *
 *  Copyright (C) 2010 Igor Krivenko <igor@shg.ru>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef KDM_HELPER_H
#define KDM_HELPER_H

#include <kauth.h>

using namespace KAuth;

class Helper : public QObject {
    Q_OBJECT

public:
    enum {
        KdmrcInstallError          = 1 << 0,
        CreateFacesDirError        = 1 << 1,
        RemoveFaceError            = 1 << 2,
        InstallFaceError           = 1 << 3,
        CreateThemesDirError       = 1 << 4,
        RemoveThemesError          = 1 << 5,
        InstallThemesError         = 1 << 6
    };

    enum { CreateFacesDir, RemoveFace, InstallFace };
    enum { CreateThemesDir, RemoveThemes, InstallThemes };

public slots:
    ActionReply save(const QVariantMap &map);
    ActionReply managefaces(const QVariantMap &map);
    ActionReply managethemes(const QVariantMap &map);

private:
    bool removeFace(const QString &facesDir, const QString &user);
    bool installFace(const QString &facesDir, const QString &user, const QString &sourceFile);
    bool removeThemes(const QString &themesDir, QStringList &themes);
    bool installThemes(const QString &themesDir, QStringList &themes);

    ActionReply createReply(int code, const QVariantMap *returnData = 0);
};

#endif // KDM_HELPER_H
