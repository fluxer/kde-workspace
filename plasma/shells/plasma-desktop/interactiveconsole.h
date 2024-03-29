/*
 *   Copyright 2009 Aaron Seigo <aseigo@kde.org>
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

#ifndef INTERACTIVECONSOLE
#define INTERACTIVECONSOLE

#include <KDialog>
#include <KIO/Job>
#include <QSplitter>
#include <QTextBrowser>
#include <QScriptValue>

class KAction;
class KFileDialog;
class KMenu;
class KTextEdit;

namespace KTextEditor
{
    class Document;
} // namespace KParts

namespace Plasma
{
    class Corona;
} // namespace Plasma

class ScriptEngine;

class InteractiveConsole : public KDialog
{
    Q_OBJECT

public:
    InteractiveConsole(Plasma::Corona *corona, QWidget *parent = 0);
    ~InteractiveConsole();

    void loadScript(const QString &path);

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *event);

protected Q_SLOTS:
    void print(const QString &string);
    void reject();

private Q_SLOTS:
    void openScriptFile();
    void saveScript();
    void scriptTextChanged();
    void evaluateScript();
    void clearEditor();
    void clearOutput();
    void scriptFileDataRecvd(KIO::Job *job, const QByteArray &data);
    void scriptFileDataReq(KIO::Job *job, QByteArray &data);
    void reenableEditor(KJob *job);
    void saveScriptUrlSelected(int result);
    void openScriptUrlSelected(int result);
    void loadScriptFromUrl(const KUrl &url);

private:
    void onClose();
    void saveScript(const KUrl &url);

    Plasma::Corona *m_corona;
    QSplitter *m_splitter;
    KTextEditor::Document *m_docEditor;
    KTextEdit *m_editor;
    QTextBrowser *m_output;
    KAction *m_loadAction;
    KAction *m_saveAction;
    KAction *m_clearAction;
    KAction *m_executeAction;

    KFileDialog *m_fileDialog;
    QWeakPointer<KIO::Job> m_job;
    bool m_closeWhenCompleted;
};

#endif

