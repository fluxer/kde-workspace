/*
 *   Copyright 2011 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "scriptenv.h"

#include <KMimeTypeTrader>
#include <KStandardDirs>
#include <KRun>
#include <KServiceTypeTrader>
#include <KShell>

QScriptValue ScriptEnv::runApplication(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        return false;
    }

    KUrl::List urls;
    if (context->argumentCount() > 1) {
        urls = qscriptvalue_cast<KUrl::List>(context->argument(1));
    }

    const QString app = context->argument(0).toString();

    const QString exec = KGlobal::dirs()->findExe(app);
    if (!exec.isEmpty()) {
        return KRun::run(exec, urls, 0);
    }

    KService::Ptr service = KService::serviceByStorageId(app);
    if (service) {
        return KRun::run(*service, urls, 0);
    }

    return false;
}

QScriptValue ScriptEnv::runCommand(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine);

    if (context->argumentCount() == 0) {
        return false;
    }

    const QString exec = KGlobal::dirs()->findExe(context->argument(0).toString());
    if (!exec.isEmpty()) {
        QString args;
        if (context->argumentCount() > 1) {
            const QStringList argList = qscriptvalue_cast<QStringList>(context->argument(1));
            if (!argList.isEmpty()) {
                args = ' ' + KShell::joinArgs(argList);
            }
        }

        return KRun::runCommand(exec + args, 0);
    }

    return false;
}

QScriptValue ScriptEnv::applicationExists(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        return false;
    }

    const QString application = context->argument(0).toString();
    if (application.isEmpty()) {
        return false;
    }

    // first, check for it in $PATH
    if (!KStandardDirs::findExe(application).isEmpty()) {
        return true;
    }

    if (KService::serviceByStorageId(application)) {
        return true;
    }

    if (application.contains("'")) {
        // apostrophes just screw up the trader lookups below, so check for it
        return false;
    }

    // next, consult ksycoca for an app by that name
    if (!KServiceTypeTrader::self()->query("Application", QString("Name =~ '%1'").arg(application)).isEmpty()) {
        return true;
    }

    // next, consult ksycoca for an app by that generic name
    if (!KServiceTypeTrader::self()->query("Application", QString("GenericName =~ '%1'").arg(application)).isEmpty()) {
        return true;
    }

    return false;
}

QScriptValue ScriptEnv::applicationPath(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        return false;
    }

    const QString application = context->argument(0).toString();
    if (application.isEmpty()) {
        return false;
    }

    // first, check for it in $PATH
    const QString path = KStandardDirs::findExe(application);
    if (!path.isEmpty()) {
        return path;
    }

    if (KService::Ptr service = KService::serviceByStorageId(application)) {
        return KStandardDirs::locate("apps", service->entryPath());
    }

    if (application.contains("'")) {
        // apostrophes just screw up the trader lookups below, so check for it
        return QString();
    }

    // next, consult ksycoca for an app by that name
    KService::List offers = KServiceTypeTrader::self()->query("Application", QString("Name =~ '%1'").arg(application));
    if (offers.isEmpty()) {
        // next, consult ksycoca for an app by that generic name
        offers = KServiceTypeTrader::self()->query("Application", QString("GenericName =~ '%1'").arg(application));
    }

    if (!offers.isEmpty()) {
        KService::Ptr offer = offers.first();
        return KStandardDirs::locate("apps", offer->entryPath());
    }

    return QString();
}

