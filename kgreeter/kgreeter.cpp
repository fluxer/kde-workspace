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

#define QT_NO_KEYWORDS
#include <QDebug>
#include <QSettings>
#include <QMainWindow>
#include <QPainter>
#include <KMessageBox>
#include <KIcon>
#include <KStandardDirs>
#include <KGlobalSettings>
#include <KSharedConfig>
#include <KLocale>

#include <glib.h>
#include <lightdm-gobject-1/lightdm.h>

#include "kgreeter.h"
#include "ui_kgreeter.h"
#include "config-workspace.h"

// for the callbacks
static const int gliblooppolltime = 200;
static GMainLoop *glibloop = NULL;

static QSettings kgreetersettings(KDE_SYSCONFDIR "/lightdm/lightdm-kgreeter-greeter.conf", QSettings::IniFormat);

class KGreeter : public QMainWindow
{
    Q_OBJECT
public:
    explicit KGreeter(QWidget *parent = nullptr);
    ~KGreeter();

    QByteArray getUser() const;
    QByteArray getPass() const;
    QByteArray getSession() const;

    LightDMGreeter* getGreeter() const;

    void clearPass();

    static void showPromptCb(LightDMGreeter *ldmgreeter, const char *ldmtext, LightDMPromptType ldmtype, gpointer ldmptr);
    static void authenticationCompleteCb(LightDMGreeter *ldmgreeter, gpointer ldmptr);
    static void showMessageCb(LightDMGreeter *ldmgreeter, const gchar *ldmtext, LightDMMessageType ldmtype, gpointer ldmptr);

    // QMainWindow reimplementations
protected:
    void paintEvent(QPaintEvent *event) final;
    void timerEvent(QTimerEvent *event) final;

private Q_SLOTS:
    void slotSuspend();
    void slotHibernate();
    void slotPoweroff();
    void slotReboot();

    void slotSession();

    void slotLayout();

    void slotLogin();

private:
    void setUser(const QString &user);
    void setSession(const QString &session);
    bool isUserLogged() const;
    static QString glibErrorString(const GError *const gliberror);

    Ui::KGreeter m_ui;
    LightDMGreeter *m_ldmgreeter;
    QList<QAction*> m_sessionactions;
    QList<QAction*> m_layoutactions;
    QImage m_background;
    QImage m_backgroundscaled;
    QImage m_rectangle;
    QImage m_rectanglescaled;
    int m_timerid;
};

KGreeter::KGreeter(QWidget *parent)
    : QMainWindow(parent),
    m_ldmgreeter(nullptr),
    m_timerid(0)
{
#if !defined(GLIB_VERSION_2_36)
    g_type_init();
#endif

    m_ui.setupUi(this);

    m_background = QImage(kgreetersettings.value("greeter/background", KGreeterDefaultBackground()).toString());
    m_rectangle = QImage(kgreetersettings.value("greeter/rectangle", KGreeterDefaultRectangle()).toString());

    m_ldmgreeter = lightdm_greeter_new();

    m_timerid = startTimer(gliblooppolltime);

    g_signal_connect(
        m_ldmgreeter, LIGHTDM_GREETER_SIGNAL_SHOW_PROMPT,
        G_CALLBACK(KGreeter::showPromptCb), this
    );
    g_signal_connect(
        m_ldmgreeter, LIGHTDM_GREETER_SIGNAL_AUTHENTICATION_COMPLETE,
        G_CALLBACK(KGreeter::authenticationCompleteCb), this
    );
    g_signal_connect(
        m_ldmgreeter, LIGHTDM_GREETER_SIGNAL_SHOW_MESSAGE,
        G_CALLBACK(KGreeter::showMessageCb), this
    );

    // TODO: sort and then add
    GList *ldmlayouts = lightdm_get_layouts();
    for (GList *ldmitem = ldmlayouts; ldmitem; ldmitem = ldmitem->next) {
        LightDMLayout *ldmlayout = static_cast<LightDMLayout*>(ldmitem->data);
        Q_ASSERT(ldmlayout);

        QAction* layoutaction = new QAction(m_ui.menuKeyboard);
        layoutaction->setCheckable(true);
        layoutaction->setText(QString::fromUtf8(lightdm_layout_get_description(ldmlayout)));
        layoutaction->setData(QVariant(QString::fromUtf8(lightdm_layout_get_name(ldmlayout))));
        connect(layoutaction, SIGNAL(triggered()), this, SLOT(slotLayout()));
        m_ui.menuKeyboard->addAction(layoutaction);
        m_layoutactions.append(layoutaction);
    }

    GList *ldmusers = lightdm_user_list_get_users(lightdm_user_list_get_instance());
    for (GList *ldmitem = ldmusers; ldmitem; ldmitem = ldmitem->next) {
        LightDMUser *ldmuser = static_cast<LightDMUser*>(ldmitem->data);
        Q_ASSERT(ldmuser);

        const QString ldmuserimage = QString::fromUtf8(lightdm_user_get_image(ldmuser));
        if (!ldmuserimage.isEmpty()) {
            m_ui.usersbox->addItem(QIcon(QPixmap(ldmuserimage)), QString::fromUtf8(lightdm_user_get_name(ldmuser)));
        } else {
            m_ui.usersbox->addItem(QString::fromUtf8(lightdm_user_get_name(ldmuser)));
        }
    }

    GList *ldmsessions = lightdm_get_sessions();
    for (GList* ldmitem = ldmsessions; ldmitem; ldmitem = ldmitem->next) {
        LightDMSession *ldmsession = static_cast<LightDMSession*>(ldmitem->data);
        Q_ASSERT(ldmsession);

        const QString ldmsessionname = QString::fromUtf8(lightdm_session_get_name(ldmsession));
        const QString ldmsessionkey = QString::fromUtf8(lightdm_session_get_key(ldmsession));

        QAction* sessionaction = new QAction(m_ui.menuSessions);
        sessionaction->setCheckable(true);
        sessionaction->setText(ldmsessionname);
        sessionaction->setData(QVariant(ldmsessionkey));
        connect(sessionaction, SIGNAL(triggered()), this, SLOT(slotSession()));
        m_ui.menuSessions->addAction(sessionaction);
        m_sessionactions.append(sessionaction);
    }

    const QString ldmdefaultuser = QString::fromUtf8(lightdm_greeter_get_select_user_hint(m_ldmgreeter));
    if (!ldmdefaultuser.isEmpty()) {
        setUser(ldmdefaultuser);
    }
    const QString ldmdefaultsession = QString::fromUtf8(lightdm_greeter_get_default_session_hint(m_ldmgreeter));
    if (!ldmdefaultsession.isEmpty()) {
        setSession(ldmdefaultsession);
    }

    QSettings kgreeterstate("lightdm-kgreeter-state");
    const QString lastuser = kgreeterstate.value("state/lastuser").toString();
    if (!lastuser.isEmpty()) {
        setUser(lastuser);
    }
    const QString lastsession = kgreeterstate.value("state/lastsession").toString();
    if (!lastsession.isEmpty()) {
        setSession(lastsession);
    }

    // if no default session is specified and no last session is saved use the first
    bool sessionchecked = false;
    Q_FOREACH (QAction *sessionaction, m_sessionactions) {
        if (sessionaction->isChecked()) {
            sessionchecked = true;
            break;
        }
    }
    if (!sessionchecked && !m_sessionactions.isEmpty()) {
        m_sessionactions.first()->setChecked(true);
    }

    m_ui.grouplabel->setText(QString::fromUtf8(lightdm_get_hostname()));

    m_ui.actionSuspend->setVisible(lightdm_get_can_suspend());
    m_ui.actionSuspend->setIcon(KIcon("system-suspend"));
    m_ui.actionHibernate->setVisible(lightdm_get_can_hibernate());
    m_ui.actionHibernate->setIcon(KIcon("system-suspend-hibernate"));
    m_ui.actionPoweroff->setVisible(lightdm_get_can_shutdown());
    m_ui.actionPoweroff->setIcon(KIcon("system-shutdown"));
    m_ui.actionReboot->setVisible(lightdm_get_can_restart());
    m_ui.actionReboot->setIcon(KIcon("system-reboot"));
    connect(m_ui.actionSuspend, SIGNAL(triggered()), this, SLOT(slotSuspend()));
    connect(m_ui.actionHibernate, SIGNAL(triggered()), this, SLOT(slotHibernate()));
    connect(m_ui.actionPoweroff, SIGNAL(triggered()), this, SLOT(slotPoweroff()));
    connect(m_ui.actionReboot, SIGNAL(triggered()), this, SLOT(slotReboot()));

    connect(m_ui.passedit, SIGNAL(returnPressed()), this, SLOT(slotLogin()));
    connect(m_ui.loginbutton, SIGNAL(pressed()), this, SLOT(slotLogin()));

    if (lightdm_greeter_get_hide_users_hint(m_ldmgreeter)
        || lightdm_greeter_get_show_manual_login_hint(m_ldmgreeter)) {
        m_ui.userlabel->setVisible(false);
        m_ui.usersbox->setVisible(false);
        m_ui.useredit->setFocus();
    } else {
        m_ui.userlabel2->setVisible(false);
        m_ui.useredit->setVisible(false);
        m_ui.passedit->setFocus();
    }
}

KGreeter::~KGreeter()
{
    killTimer(m_timerid);
}

void KGreeter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (!m_background.isNull()) {
        const QSize windowsize = size();
        if (m_backgroundscaled.isNull() || windowsize != m_backgroundscaled.size()) {
            m_backgroundscaled = m_background.scaled(windowsize);
        }
        painter.drawImage(QPoint(0, 0), m_backgroundscaled);
    }

    if (!m_rectangle.isNull()) {
        m_ui.groupframe->setFrameShape(QFrame::NoFrame);
        QSize kgreeterrectanglesize(m_ui.groupframe->size());
        kgreeterrectanglesize.rwidth() = kgreeterrectanglesize.width() * 1.06;
        kgreeterrectanglesize.rheight() = kgreeterrectanglesize.height() * 1.65;
        QPointF kgreeterrectanglepoint(m_ui.groupframe->pos());
        kgreeterrectanglepoint -= QPointF(kgreeterrectanglepoint.x() * 0.03 , 0);
        if (m_rectanglescaled.isNull() || kgreeterrectanglesize != m_rectanglescaled.size()) {
            m_rectanglescaled = m_rectangle.scaled(kgreeterrectanglesize);
        }
        painter.drawImage(kgreeterrectanglepoint, m_rectanglescaled);
    } else {
        m_ui.groupframe->setFrameShape(QFrame::StyledPanel);
    }

    QMainWindow::paintEvent(event);
}

void KGreeter::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerid) {
        g_main_context_iteration(g_main_loop_get_context(glibloop), false);
    }
    QMainWindow::timerEvent(event);
}

QByteArray KGreeter::getUser() const
{
    if (m_ui.useredit->isVisible()) {
        return m_ui.useredit->text().toUtf8();
    }
    return m_ui.usersbox->currentText().toUtf8();
}

QByteArray KGreeter::getPass() const
{
    return m_ui.passedit->text().toUtf8();
}

QByteArray KGreeter::getSession() const
{
    Q_FOREACH (const QAction *sessionaction, m_sessionactions) {
        if (sessionaction->isChecked()) {
            return sessionaction->data().toByteArray();
        }
    }
    Q_ASSERT(false);
    return QByteArray();
}

LightDMGreeter * KGreeter::getGreeter() const
{
    return m_ldmgreeter;
}

void KGreeter::clearPass()
{
    m_ui.passedit->clear();
    if (m_ui.useredit->isVisible()) {
        m_ui.useredit->setFocus();
    } else {
        m_ui.passedit->setFocus();
    }
}

void KGreeter::showPromptCb(LightDMGreeter *ldmgreeter, const char *ldmtext, LightDMPromptType ldmtype, gpointer ldmptr)
{
    // qDebug() << Q_FUNC_INFO;

    KGreeter* kgreeter = static_cast<KGreeter*>(ldmptr);
    Q_ASSERT(kgreeter);

    if (ldmtype == LIGHTDM_PROMPT_TYPE_SECRET) {
        const QByteArray kgreeterpass = kgreeter->getPass();

        g_autoptr(GError) gliberror = NULL;
        if (!lightdm_greeter_respond(ldmgreeter, kgreeterpass.constData(), &gliberror)) {
            kgreeter->statusBar()->showMessage(i18n("Failed to repsond: %1", KGreeter::glibErrorString(gliberror)));
        }
    }
}

void KGreeter::authenticationCompleteCb(LightDMGreeter *ldmgreeter, gpointer ldmptr)
{
    // qDebug() << Q_FUNC_INFO;

    KGreeter* kgreeter = static_cast<KGreeter*>(ldmptr);
    Q_ASSERT(kgreeter);

    const QByteArray kgreetersession = kgreeter->getSession();

    if (!lightdm_greeter_get_is_authenticated(ldmgreeter)) {
        kgreeter->statusBar()->showMessage(i18n("Failed to authenticate"));
        kgreeter->clearPass();
        return;
    }

    g_autoptr(GError) gliberror = NULL;
    if (!lightdm_greeter_start_session_sync(ldmgreeter, kgreetersession.constData(), &gliberror)) {
        kgreeter->statusBar()->showMessage(i18n("Failed to start session: %1", KGreeter::glibErrorString(gliberror)));
        kgreeter->clearPass();
        return;
    }

    qApp->quit();
}

void KGreeter::showMessageCb(LightDMGreeter *ldmgreeter, const gchar *ldmtext, LightDMMessageType ldmtype, gpointer ldmptr)
{
    // qDebug() << Q_FUNC_INFO;

    KGreeter* kgreeter = static_cast<KGreeter*>(ldmptr);
    Q_ASSERT(kgreeter);

    kgreeter->statusBar()->showMessage(QString::fromUtf8(ldmtext));
}

void KGreeter::slotSuspend()
{
    g_autoptr(GError) gliberror = NULL;
    if (!lightdm_suspend(&gliberror)) {
        statusBar()->showMessage(i18n("Could not suspend: %1", KGreeter::glibErrorString(gliberror)));
    }
}

void KGreeter::slotHibernate()
{
    g_autoptr(GError) gliberror = NULL;
    if (!lightdm_hibernate(&gliberror)) {
        statusBar()->showMessage(i18n("Could not hibernate: %1", KGreeter::glibErrorString(gliberror)));
    }
}

void KGreeter::slotPoweroff()
{
    if (isUserLogged()) {
        const int kmessageresult = KMessageBox::questionYesNo(
            this,
            i18n("There is user logged in, are you sure you want to poweroff?")
        );
        if (kmessageresult != KMessageBox::Yes) {
            return;
        }
    }

    g_autoptr(GError) gliberror = NULL;
    if (!lightdm_shutdown(&gliberror)) {
        statusBar()->showMessage(i18n("Could not poweroff: %1", KGreeter::glibErrorString(gliberror)));
    }
}

void KGreeter::slotReboot()
{
    if (isUserLogged()) {
        const int kmessageresult = KMessageBox::questionYesNo(
            this,
            i18n("There is user logged in, are you sure you want to reboot?")
        );
        if (kmessageresult != KMessageBox::Yes) {
            return;
        }
    }

    g_autoptr(GError) gliberror = NULL;
    if (!lightdm_restart(&gliberror)) {
        statusBar()->showMessage(i18n("Could not reboot: %1", KGreeter::glibErrorString(gliberror)));
    }
}

void KGreeter::slotSession()
{
    const QAction* sessionaction = qobject_cast<QAction*>(sender());
    const QString sessionname = sessionaction->data().toString();

    Q_FOREACH (QAction *sessionaction, m_sessionactions) {
        sessionaction->setChecked(false);
        if (sessionaction->data().toString() == sessionname) {
            sessionaction->setChecked(true);
        }
    }
}

void KGreeter::slotLayout()
{
    QString ldmlayoutname;

    const QAction* layoutaction = qobject_cast<QAction*>(sender());
    const QString layoutname = layoutaction->data().toString();
    GList *ldmlayouts = lightdm_get_layouts();
    for (GList *ldmitem = ldmlayouts; ldmitem; ldmitem = ldmitem->next) {
        LightDMLayout *ldmlayout = static_cast<LightDMLayout*>(ldmitem->data);
        Q_ASSERT(ldmlayout);

        ldmlayoutname = QString::fromUtf8(lightdm_layout_get_name(ldmlayout));
        if (layoutname == ldmlayoutname) {
            lightdm_set_layout(ldmlayout);
            break;
        }
    }

    if (ldmlayoutname.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    Q_FOREACH (QAction *layoutaction, m_layoutactions) {
        layoutaction->setChecked(false);
        if (layoutaction->data().toString() == ldmlayoutname) {
            layoutaction->setChecked(true);
        }
    }
}

void KGreeter::slotLogin()
{
    const QByteArray kgreeterusername = getUser();
    const QByteArray kgreetersession = getSession();

    // the trick is to save before lightdm_greeter_authenticate()
    {
        QSettings kgreeterstate("lightdm-kgreeter-state");
        kgreeterstate.setValue("state/lastsession", kgreetersession);
        kgreeterstate.setValue("state/lastuser", kgreeterusername);
    }

    g_autoptr(GError) gliberror = NULL;
    lightdm_greeter_authenticate(m_ldmgreeter, kgreeterusername.constData(), &gliberror);
}

void KGreeter::setUser(const QString &user)
{
    for (int i = 0; i < m_ui.usersbox->count(); i++) {
        if (m_ui.usersbox->itemText(i) == user) {
            m_ui.usersbox->setCurrentIndex(i);
            break;
        }
    }
    m_ui.useredit->setText(user);
}

void KGreeter::setSession(const QString &session)
{
    Q_FOREACH (QAction *sessionaction, m_sessionactions) {
        sessionaction->setChecked(false);
        if (sessionaction->data().toString() == session) {
            sessionaction->setChecked(true);
        }
    }
}

bool KGreeter::isUserLogged() const
{
    GList *ldmusers = lightdm_user_list_get_users(lightdm_user_list_get_instance());
    for (GList *ldmitem = ldmusers; ldmitem; ldmitem = ldmitem->next) {
        LightDMUser *ldmuser = static_cast<LightDMUser*>(ldmitem->data);
        Q_ASSERT(ldmuser);

        if (lightdm_user_get_logged_in(ldmuser)) {
            return true;
        }
    }
    return false;
}

QString KGreeter::glibErrorString(const GError *const gliberror)
{
    if (!gliberror) {
        return i18n("Unknown error");
    }
    return QString::fromUtf8(gliberror->message);
}

int main(int argc, char**argv)
{
    QApplication app(argc, argv);

    const QString kgreeterfontstring = kgreetersettings.value("greeter/font").toString();
    QFont kgreeterfont;
    if (!kgreeterfont.fromString(kgreeterfontstring)) {
        kgreeterfont = KGreeterDefaultFont();
    }
    app.setFont(kgreeterfont);

    // for the style
    const QStringList pluginpaths = KGlobal::dirs()->resourceDirs("qtplugins");
    Q_FOREACH (const QString &path, pluginpaths) {
        app.addLibraryPath(path);
    }

    const QString kgreeterstyle = kgreetersettings.value("greeter/style", KGreeterDefaultStyle()).toString();
    app.setStyle(kgreeterstyle);

    const QString kgreetercolorscheme = kgreetersettings.value("greeter/colorscheme").toString();
    if (!kgreetercolorscheme.isEmpty()) {
        KSharedConfigPtr kcolorschemeconfig = KSharedConfig::openConfig(
            QString::fromLatin1("color-schemes/%1.colors").arg(kgreetercolorscheme),
            KConfig::FullConfig, "data"
        );
        app.setPalette(KGlobalSettings::createApplicationPalette(kcolorschemeconfig));
    } else {
        app.setPalette(KGlobalSettings::createApplicationPalette());
    }

    glibloop = g_main_loop_new(NULL, false);

    KGreeter kgreeter;
    kgreeter.showMaximized();

    LightDMGreeter *ldmgreeter = kgreeter.getGreeter();

    g_autoptr(GError) gliberror = NULL;
    if (!lightdm_greeter_connect_to_daemon_sync(ldmgreeter, &gliberror)) {
        ::fprintf(stderr, "%s: %s\n", "Could not connect to daemon", gliberror->message);
        return 1;
    }

    return app.exec();
}

#include "kgreeter.moc"
