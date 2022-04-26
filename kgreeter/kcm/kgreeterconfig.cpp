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
#include "kgreeter.h"

#include <QSettings>
#include <QStyleFactory>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <klocale.h>
#include <kauthaction.h>
#include <kimageio.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kstyle.h>
#include <kglobalsettings.h>
#include <kaboutdata.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

#include <sys/types.h>
#include <signal.h>

#include "config-workspace.h"

K_PLUGIN_FACTORY(KCMGreeterFactory, registerPlugin<KCMGreeter>();)
K_EXPORT_PLUGIN(KCMGreeterFactory("kcmgreeterconfig", "kcm_greeterconfig"))

KCMGreeter::KCMGreeter(QWidget* parent, const QVariantList& args)
    : KCModule(KCMGreeterFactory::componentData(), parent),
    m_lightdmexe(KStandardDirs::findRootExe("lightdm")),
    m_lightdmpid(0)
{
    Q_UNUSED(args);

    setQuickHelp(i18n("<h1>KGreeter</h1> This module allows you to change KDE greeter options."));

    setupUi(this);

    KAboutData *about = new KAboutData(
        I18N_NOOP("kcmkgreeter"), 0,
        ki18n("KDE Greeter Module"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("Copyright 2022, Ivailo Monev <email>xakepa10@gmail.com</email>")
    );
    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    setNeedsAuthorization(true);

    load();

    connect(fontchooser, SIGNAL(fontSelected(QFont)), this, SLOT(slotFontChanged(QFont)));

    const QStringList kthemercs = KGlobal::dirs()->findAllResources("data", "kstyle/themes/*.themerc");
    foreach (const QString &style, QStyleFactory::keys()) {
        QString kthemename = style;
        foreach (const QString &kthemerc, kthemercs) {
            KConfig kconfig(kthemerc, KConfig::SimpleConfig);
            const QString kthemercwidgetstyle = kconfig.group("KDE").readEntry("WidgetStyle");
            if (kthemercwidgetstyle.isEmpty() || kthemercwidgetstyle.toLower() != style.toLower()) {
                continue;
            }
            kthemename = kconfig.group("Misc").readEntry("Name");
        }
        stylesbox->addItem(kthemename, QVariant(style));
    }
    connect(stylesbox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotStyleChanged(QString)));

    colorsbox->addItem(i18n("Default"), QVariant(QString::fromLatin1("default")));
    const QStringList kcolorschemes = KGlobal::dirs()->findAllResources("data", "color-schemes/*.colors");
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

    testbutton->setIcon(KIcon("debug-run"));
    connect(testbutton, SIGNAL(pressed()), this, SLOT(slotTest()));
}

KCMGreeter::~KCMGreeter()
{
    killLightDM();
}

void KCMGreeter::load()
{
    QSettings kgreetersettings(KDE_SYSCONFDIR "/lightdm/lightdm-kgreeter-greeter.conf", QSettings::IniFormat);

    const QString kgreeterfontstring = kgreetersettings.value("greeter/font").toString();
    QFont kgreeterfont;
    if (!kgreeterfont.fromString(kgreeterfontstring)) {
        kgreeterfont = KGreeterDefaultFont();
    }
    fontchooser->setFont(kgreeterfont);

    const QString kgreeterstyle = kgreetersettings.value("greeter/style", KGreeterDefaultStyle()).toString();
    for (int i = 0; i < stylesbox->count(); i++) {
        if (stylesbox->itemData(i).toString().toLower() == kgreeterstyle.toLower()) {
            stylesbox->setCurrentIndex(i);
            break;
        }
    }

    colorsbox->setCurrentIndex(0); // default
    const QString kgreetercolor = kgreetersettings.value("greeter/colorscheme").toString();
    if (!kgreetercolor.isEmpty()) {
        for (int i = 0; i < colorsbox->count(); i++) {
            if (colorsbox->itemData(i).toString().toLower() == kgreetercolor.toLower()) {
                colorsbox->setCurrentIndex(i);
                break;
            }
        }
    }

    const QString kgreeterbackground = kgreetersettings.value("greeter/background", KGreeterDefaultBackground()).toString();
    backgroundrequester->setUrl(KUrl(kgreeterbackground));

    const QString kgreeterrectangle = kgreetersettings.value("greeter/rectangle", KGreeterDefaultRectangle()).toString();
    rectanglerequester->setUrl(KUrl(kgreeterrectangle));

    enableTest(true);
    emit changed(false);
}

void KCMGreeter::save()
{
    KAuth::Action kgreeteraction("org.kde.kcontrol.kcmkgreeter.save");
    kgreeteraction.setHelperID("org.kde.kcontrol.kcmkgreeter");
    kgreeteraction.addArgument("font", fontchooser->font().toString());
    kgreeteraction.addArgument("style", stylesbox->itemData(stylesbox->currentIndex()).toString());
    kgreeteraction.addArgument("colorscheme", colorsbox->itemData(colorsbox->currentIndex()).toString());
    kgreeteraction.addArgument("background", backgroundrequester->url().path());
    kgreeteraction.addArgument("rectangle", rectanglerequester->url().path());
    KAuth::ActionReply kgreeterreply = kgreeteraction.execute();

    // qDebug() << kgreeter.errorCode() << kgreeter.errorDescription();

    if (kgreeterreply != KAuth::ActionReply::SuccessReply) {
        KMessageBox::error(this, kgreeterreply.errorDescription());
    }

    enableTest(true);
    emit changed(false);
}

void KCMGreeter::defaults()
{
    fontchooser->setFont(KGlobalSettings::generalFont());
    for (int i = 0; i < stylesbox->count(); i++) {
        if (stylesbox->itemData(i).toString().toLower() == KStyle::defaultStyle().toLower()) {
            stylesbox->setCurrentIndex(i);
            break;
        }
    }
    colorsbox->setCurrentIndex(0);
    backgroundrequester->setUrl(KUrl());
    rectanglerequester->setUrl(KUrl());

    enableTest(false);
    emit changed(true);
}

void KCMGreeter::slotFontChanged(const QFont &font)
{
    Q_UNUSED(font);
    enableTest(false);
    emit changed(true);
}

void KCMGreeter::slotStyleChanged(const QString &style)
{
    Q_UNUSED(style);
    enableTest(false);
    emit changed(true);
}

void KCMGreeter::slotColorChanged(const QString &color)
{
    Q_UNUSED(color);
    enableTest(false);
    emit changed(true);
}

void KCMGreeter::slotURLChanged(const QString &url)
{
    Q_UNUSED(url);
    enableTest(false);
    emit changed(true);
}

void KCMGreeter::slotURLChanged(const KUrl &url)
{
    Q_UNUSED(url);
    enableTest(false);
    emit changed(true);
}

void KCMGreeter::slotTest()
{
    killLightDM();

    const bool result = QProcess::startDetached(
        m_lightdmexe,
        QStringList() << QString::fromLatin1("--test-mode"),
        QDir::currentPath(),
        &m_lightdmpid
    );
    if (!result) {
        KMessageBox::error(this, i18n("Could not start LightDM"));
    }
}

void KCMGreeter::enableTest(const bool enable)
{
    if (enable) {
        testbutton->setEnabled(!m_lightdmexe.isEmpty());
    } else {
        testbutton->setEnabled(false);
    }
}

void KCMGreeter::killLightDM()
{
    if (m_lightdmpid > 0) {
        ::kill(pid_t(m_lightdmpid), SIGTERM);
    }
}

#include "moc_kgreeterconfig.cpp"
