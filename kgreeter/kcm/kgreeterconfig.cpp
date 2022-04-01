/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kgreeterconfig.h"

#include <QSettings>
#include <QStyleFactory>
#include <QProcess>
#include <kdebug.h>
#include <klocale.h>
#include <kauthaction.h>
#include <kimageio.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kaboutdata.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

#include "config-workspace.h"

K_PLUGIN_FACTORY(KCMGreeterFactory, registerPlugin<KCMGreeter>();)
K_EXPORT_PLUGIN(KCMGreeterFactory("kcmgreeterconfig", "kcm_greeterconfig"))

KCMGreeter::KCMGreeter(QWidget* parent, const QVariantList& args)
    : KCModule(KCMGreeterFactory::componentData(), parent)
{
    Q_UNUSED(args);

    setQuickHelp(i18n("<h1>KGreeter</h1> This module allows you to change KDE greeter options."));

    setupUi(this);

    KAboutData *about =
        new KAboutData(I18N_NOOP("kcmkgreeter"), 0,
                       ki18n("KDE Greeter Module"),
                       0, KLocalizedString(), KAboutData::License_GPL,
                       ki18n("Copyright 2022, Ivailo Monev <email>xakepa10@gmail.com</email>"
                       ));

    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    setNeedsAuthorization(true);
    setButtons(KCModule::Help | KCModule::Apply);

    load();

    stylesbox->addItems(QStyleFactory::keys());
    connect(stylesbox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotStyleChanged(QString)));

    const QStringList kcolorschemes = KGlobal::dirs()->findAllResources("data", "color-schemes/*.colors", KStandardDirs::NoDuplicates);
    foreach (const QString &kcolorscheme, kcolorschemes) {
        const QString kcolorschemename = QSettings(kcolorscheme, QSettings::IniFormat).value("General/Name").toString();
        const QString kcolorschemebasename = QFileInfo(kcolorscheme).baseName();
        colorsbox->addItem(kcolorschemename, QVariant(kcolorschemebasename));
    }
    connect(colorsbox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotColorChanged(QString)));

    backgroundrequester->setFilter(KImageIO::pattern(KImageIO::Reading));
    connect(backgroundrequester, SIGNAL(textChanged(QString)), this, SLOT(slotURLChanged(QString)));
    connect(backgroundrequester, SIGNAL(urlSelected(KUrl)), this, SLOT(slotURLChanged(KUrl)));

    rectanglerequester->setFilter(KImageIO::pattern(KImageIO::Reading));
    connect(rectanglerequester, SIGNAL(textChanged(QString)), this, SLOT(slotURLChanged(QString)));
    connect(rectanglerequester, SIGNAL(urlSelected(KUrl)), this, SLOT(slotURLChanged(KUrl)));

    m_lightdmexe = KStandardDirs::findRootExe("lightdm");
    testbutton->setEnabled(!m_lightdmexe.isEmpty());
    testbutton->setIcon(KIcon("debug-run"));
    connect(testbutton, SIGNAL(pressed()), this, SLOT(slotTest()));
}

KCMGreeter::~KCMGreeter()
{
}

void KCMGreeter::load()
{
    QSettings kgreetersettings(KDE_SYSCONFDIR "/lightdm/lightdm-kgreeter-greeter.conf", QSettings::IniFormat);

    const QString kgreeterstyle = kgreetersettings.value("greeter/style").toString();
    if (!kgreeterstyle.isEmpty()) {
        for (int i = 0; i < stylesbox->count(); i++) {
            if (stylesbox->itemText(i) == kgreeterstyle) {
                stylesbox->setCurrentIndex(i);
                break;
            }
        }
    }

    const QString kgreetercolor = kgreetersettings.value("greeter/colorscheme").toString();
    if (!kgreetercolor.isEmpty()) {
        for (int i = 0; i < colorsbox->count(); i++) {
            if (colorsbox->itemData(i).toString() == kgreetercolor) {
                colorsbox->setCurrentIndex(i);
                break;
            }
        }
    }

    const QString kgreeterbackground = kgreetersettings.value("greeter/background").toString();
    backgroundrequester->setUrl(KUrl(kgreeterbackground));

    const QString kgreeterrectangle = kgreetersettings.value("greeter/rectangle").toString();
    rectanglerequester->setUrl(KUrl(kgreeterrectangle));

    emit changed(false);
}

void KCMGreeter::save()
{
    KAuth::Action kgreeteraction("org.kde.kcontrol.kcmkgreeter.save");
    kgreeteraction.setHelperID("org.kde.kcontrol.kcmkgreeter");
    kgreeteraction.addArgument("style", stylesbox->currentText());
    kgreeteraction.addArgument("colorscheme", colorsbox->itemData(colorsbox->currentIndex()).toString());
    kgreeteraction.addArgument("background", backgroundrequester->url().path());
    kgreeteraction.addArgument("rectangle", rectanglerequester->url().path());
    KAuth::ActionReply kgreeterreply = kgreeteraction.execute();

    // qDebug() << kgreeter.errorCode() << kgreeter.errorDescription();

    if (kgreeterreply != KAuth::ActionReply::SuccessReply) {
        KMessageBox::error(this, kgreeterreply.errorDescription());
    }
    emit changed(false);
}

void KCMGreeter::slotStyleChanged(const QString &style)
{
    Q_UNUSED(style);
    emit changed(true);
}

void KCMGreeter::slotColorChanged(const QString &color)
{
    Q_UNUSED(color);
    emit changed(true);
}

void KCMGreeter::slotURLChanged(const QString &url)
{
    Q_UNUSED(url);
    emit changed(true);
}

void KCMGreeter::slotURLChanged(const KUrl &url)
{
    Q_UNUSED(url);
    emit changed(true);
}

void KCMGreeter::slotTest()
{
    if (!QProcess::startDetached(m_lightdmexe, QStringList() << QString::fromLatin1("--test-mode"))) {
        KMessageBox::error(this, i18n("Could not start LightDM"));
    }
}

#include "moc_kgreeterconfig.cpp"
