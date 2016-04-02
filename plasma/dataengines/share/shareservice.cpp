/***************************************************************************
 *   Copyright 2010 Artur Duque de Souza <asouza@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include <kstandarddirs.h>

#include <Plasma/Package>

#include "shareservice.h"
#include "shareprovider.h"


ShareService::ShareService(ShareEngine *engine)
    : Plasma::Service(engine)
{
    setName("share");
}

Plasma::ServiceJob *ShareService::createJob(const QString &operation,
                                            QMap<QString, QVariant> &parameters)
{
    return new ShareJob(destination(), operation, parameters, this);
}

ShareJob::ShareJob(const QString &destination, const QString &operation,
                   QMap<QString, QVariant> &parameters, QObject *parent)
    : Plasma::ServiceJob(destination, operation, parameters, parent),
      m_engine(0), m_provider(0), m_package(0)
{
}

ShareJob::~ShareJob()
{
    delete m_engine;
    delete m_provider;
    delete m_package;
}

void ShareJob::start()
{
    //KService::Ptr service = KService::serviceByStorageId("plasma-share-pastebincom.desktop");
    KService::Ptr service = KService::serviceByStorageId(destination());
    if (!service) {
        showError(i18n("Could not find the provider with the specified destination"));
        return;
    }

    QString pluginName =
        service->property("X-KDE-PluginInfo-Name", QVariant::String).toString();

    const QString path =
        KStandardDirs::locate("data", "plasma/shareprovider/" + pluginName + '/' );

    if (path.isEmpty()) {
        showError(i18n("Invalid path for the requested provider"));
        return;
    }

    m_package = new Plasma::Package(path, ShareProvider::packageStructure());
    if (m_package->isValid()) {
        const QString mainscript =
            m_package->path() + m_package->structure()->contentsPrefixPaths().at(0) +
            m_package->structure()->path("mainscript");

        QFile mainfile(mainscript);
        if (!mainfile.exists()) {
            showError(i18n("Selected provider does not have a valid script file"));
            return;
        }
        mainfile.open(QIODevice::ReadOnly);

        QScriptEngine * m_engine = new QScriptEngine(parent());
        if (!m_engine) {
            showError(i18n("Could not initialize engine"));
            return;
        }

        m_provider = new ShareProvider(this);
        connect(m_provider, SIGNAL(readyToPublish()), this, SLOT(publish()));
        connect(m_provider, SIGNAL(finished(QString)),
                this, SLOT(showResult(QString)));
        connect(m_provider, SIGNAL(finishedError(QString)),
                this, SLOT(showError(QString)));

        QScriptValue provideproto = m_engine->newQObject(m_provider);
        m_engine->globalObject().setProperty("provider", provideproto);

        QScriptValue maincode = m_engine->evaluate(mainfile.readAll(), mainscript);

        if(m_engine->hasUncaughtException()) {
            showError(i18n("Error trying to execute script"));
            return;
        }

        QScriptValue mainobject = m_engine->globalObject();
        // do the work together with the loaded plugin
        if (!mainobject.property("url").isFunction()
            || !mainobject.property("contentKey").isFunction()
            || !mainobject.property("setup").isFunction()) {
            showError(i18n("Could not find all required functions"));
            return;
        }

        // call the methods from the plugin
        const QString url = mainobject.property("url").call().toString();
        m_provider->setUrl(url);

        // default is POST (if the plugin does not specify one method)
        QVariant method = "POST";
        if (mainobject.property("method").isFunction()) {
            method = mainobject.property("method").call().toString();
        }
        m_provider->setMethod(method.toString());

        // setup the provider
        QScriptValue setup = mainobject.property("setup").call();

        // get the content from the parameters, set the url and add the file
        // then we can wait the signal to publish the information
        const QString contentKey = mainobject.property("contentKey").call().toString();

        const QString content(parameters()["content"].toString());
        m_provider->addPostFile(contentKey, content);
    }
}

void ShareJob::publish()
{
    m_provider->publish();
}

void ShareJob::showResult(const QString &url)
{
    setResult(url);
}

void ShareJob::showError(const QString &message)
{
    QString errorMsg = message;
    if (errorMsg.isEmpty()) {
        errorMsg = i18n("Unknown Error");
    }

    setError(1);
    setErrorText(message);
    emitResult();
}

#include "moc_shareservice.cpp"
