/*
 *   Copyright (C) 2006 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
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

#include "shellrunner.h"

#include <QWidget>
#include <QPushButton>

#include <KDebug>
#ifdef Q_OS_UNIX
#include <SuProcess>
#endif
#include <KIcon>
#include <KLocale>
#include <KRun>
#include <KShell>
#include <KStandardDirs>
#include <KToolInvocation>

#include <Plasma/Theme>

#include "shell_config.h"

ShellRunner::ShellRunner(QObject *parent, const QVariantList &args)
    : Plasma::AbstractRunner(parent, args),
      m_inTerminal(false)
{
    setObjectName( QLatin1String("Command" ));
    setPriority(AbstractRunner::HighestPriority);
    setHasRunOptions(true);
    setIgnoredTypes(Plasma::RunnerContext::Directory | Plasma::RunnerContext::File |
                    Plasma::RunnerContext::NetworkLocation | Plasma::RunnerContext::UnknownType |
                    Plasma::RunnerContext::Help);

    addSyntax(Plasma::RunnerSyntax(":q:", i18n("Finds commands that match :q:, using common shell syntax")));
}

ShellRunner::~ShellRunner()
{
}

void ShellRunner::match(Plasma::RunnerContext &context)
{
    if (context.type() == Plasma::RunnerContext::Executable ||
        context.type() == Plasma::RunnerContext::ShellCommand)  {
        const QString term = context.query();
        Plasma::QueryMatch match(this);
        match.setId(term);
        match.setType(Plasma::QueryMatch::ExactMatch);
        match.setIcon(KIcon("system-run"));
        match.setText(i18n("Run %1", term));
        match.setRelevance(0.7);
        context.addMatch(term, match);
    }
}

void ShellRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(match);

    // filter match's id to remove runner's name
    // as this is the command we want to run

    if (m_inTerminal) {
        KToolInvocation::invokeTerminal(context.query());
    } else {
        KRun::runCommand(context.query(), NULL);
    }

    // reset for the next run!
    m_inTerminal = false;
}

void ShellRunner::createRunOptions(QWidget *parent)
{
    //TODO: for multiple runners?
    //TODO: sync palette on theme changes
    ShellConfig *configWidget = new ShellConfig(config(), parent);

    QPalette pal = configWidget->palette();
    Plasma::Theme *theme = Plasma::Theme::defaultTheme();
    pal.setColor(QPalette::Normal, QPalette::Window, theme->color(Plasma::Theme::BackgroundColor));
    pal.setColor(QPalette::Normal, QPalette::WindowText, theme->color(Plasma::Theme::TextColor));
    configWidget->setPalette(pal);

    connect(configWidget->m_ui.cbRunInTerminal, SIGNAL(clicked(bool)), this, SLOT(setRunInTerminal(bool)));
}

void ShellRunner::setRunInTerminal(bool runInTerminal)
{
    m_inTerminal = runInTerminal;
}

#include "moc_shellrunner.cpp"
