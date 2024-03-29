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

#include "interactiveconsole.h"

#include <QElapsedTimer>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTextBrowser>

#include <KFileDialog>
#include <KLocale>
#include <KAction>
#include <KShell>
#include <KMessageBox>
#include <KMenu>
#include <KServiceTypeTrader>
#include <KStandardAction>
#include <KStandardDirs>
#include <KTextEdit>
#include <KTextEditor/EditorChooser>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KToolBar>

#include <Plasma/Corona>
#include <Plasma/Package>

#include "plasmaapp.h"
#include "scripting/desktopscriptengine.h"

// TODO: interative help?
static const QString s_autosaveFileName("interactiveconsoleautosave.js");

InteractiveConsole::InteractiveConsole(Plasma::Corona *corona, QWidget *parent)
    : KDialog(parent),
      m_corona(corona),
      m_splitter(new QSplitter(Qt::Vertical, this)),
      m_docEditor(0),
      m_editor(0),
      m_loadAction(KStandardAction::open(this, SLOT(openScriptFile()), this)),
      m_saveAction(KStandardAction::saveAs(this, SLOT(saveScript()), this)),
      m_clearAction(KStandardAction::clear(this, SLOT(clearEditor()), this)),
      m_executeAction(new KAction(KIcon("system-run"), i18n("&Execute"), this)),
      m_fileDialog(0)
{
    addAction(KStandardAction::close(this, SLOT(close()), this));
    addAction(m_saveAction);
    addAction(m_clearAction);

    setWindowTitle(KDialog::makeStandardCaption(i18n("Desktop Shell Scripting Console")));
    setAttribute(Qt::WA_DeleteOnClose);
    setButtons(KDialog::None);

    QWidget *widget = new QWidget(m_splitter);
    QVBoxLayout *editorLayout = new QVBoxLayout(widget);

    QLabel *label = new QLabel(i18n("Editor"), widget);
    QFont f = label->font();
    f.setBold(true);
    label->setFont(f);
    editorLayout->addWidget(label);

    KToolBar *toolBar = new KToolBar(this, true, false);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addAction(m_loadAction);
    toolBar->addAction(m_saveAction);
    toolBar->addAction(m_clearAction);
    toolBar->addAction(m_executeAction);

    editorLayout->addWidget(toolBar);

    m_docEditor = KTextEditor::EditorChooser::editor()->createDocument(widget);
    if (m_docEditor) {
        m_docEditor->setHighlightingMode("JavaScript/PlasmaDesktop");

        KTextEditor::View * view = m_docEditor->createView(widget);
        view->setContextMenu(view->defaultContextMenu());

        KTextEditor::ConfigInterface *config = qobject_cast<KTextEditor::ConfigInterface*>(view);
        if (config) {
            config->setConfigValue("line-numbers", true);
            config->setConfigValue("dynamic-word-wrap", true);
        }

        editorLayout->addWidget(view);
        connect(m_docEditor, SIGNAL(textChanged(KTextEditor::Document*)),
                this, SLOT(scriptTextChanged()));
    }

    if (!m_docEditor) {
        m_editor = new KTextEdit(widget);
        editorLayout->addWidget(m_editor);
        connect(m_editor, SIGNAL(textChanged()), this, SLOT(scriptTextChanged()));
    }

    m_splitter->addWidget(widget);

    widget = new QWidget(m_splitter);
    QVBoxLayout *outputLayout = new QVBoxLayout(widget);

    label = new QLabel(i18n("Output"), widget);
    f = label->font();
    f.setBold(true);
    label->setFont(f);
    outputLayout->addWidget(label);

    KToolBar *outputToolBar = new KToolBar(widget, true, false);
    outputToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QAction *clearOutputAction = KStandardAction::clear(this, SLOT(clearOutput()), this);
    outputToolBar->addAction(clearOutputAction);
    outputLayout->addWidget(outputToolBar);

    m_output = new QTextBrowser(widget);
    outputLayout->addWidget(m_output);
    m_splitter->addWidget(widget);

    setMainWidget(m_splitter);

    setInitialSize(QSize(700, 500));
    KConfigGroup cg(KGlobal::config(), "InteractiveConsole");
    restoreDialogSize(cg);

    m_splitter->setStretchFactor(0, 10);
    m_splitter->restoreState(cg.readEntry("SplitterState", QByteArray()));

    scriptTextChanged();

    connect(m_executeAction, SIGNAL(triggered()), this, SLOT(evaluateScript()));
    m_executeAction->setShortcut(Qt::CTRL + Qt::Key_E);

    const QString autosave = KStandardDirs::locateLocal("appdata", s_autosaveFileName);
    if (QFile::exists(autosave)) {
        loadScript(autosave);
    }
}

InteractiveConsole::~InteractiveConsole()
{
    KConfigGroup cg(KGlobal::config(), "InteractiveConsole");
    saveDialogSize(cg);
    cg.writeEntry("SplitterState", m_splitter->saveState());
    kDebug();
}

void InteractiveConsole::loadScript(const QString &script)
{
    if (m_docEditor) {
        m_docEditor->closeUrl(false);
        if (m_docEditor->openUrl(script)) {
            m_docEditor->setHighlightingMode("JavaScript/PlasmaDesktop");
            return;
        }
    } else {
        QFile file(KShell::tildeExpand(script));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
            m_editor->setText(file.readAll());
            return;
        }
    }


    m_output->append(i18n("Unable to load script file <b>%1</b>", script));
}

void InteractiveConsole::showEvent(QShowEvent *)
{
    if (m_docEditor) {
        m_docEditor->activeView()->setFocus();
    } else {
        m_editor->setFocus();
    }
}

void InteractiveConsole::closeEvent(QCloseEvent *event)
{
    onClose();
    KDialog::closeEvent(event);
}

void InteractiveConsole::reject()
{
    onClose();
    KDialog::reject();
}

void InteractiveConsole::onClose()
{
    // need to save first!
    const QString path = KStandardDirs::locateLocal("appdata", s_autosaveFileName);
    m_closeWhenCompleted = true;
    saveScript(path);
}

void InteractiveConsole::print(const QString &string)
{
    m_output->append(string);
}

void InteractiveConsole::scriptTextChanged()
{
    const bool enable = m_docEditor ? !m_docEditor->isEmpty() : !m_editor->document()->isEmpty();
    m_saveAction->setEnabled(enable);
    m_clearAction->setEnabled(enable);
    m_executeAction->setEnabled(enable);
}

void InteractiveConsole::openScriptFile()
{
    delete m_fileDialog;

    m_fileDialog = new KFileDialog(KUrl(), QString(), 0);
    m_fileDialog->setOperationMode(KFileDialog::Opening);
    m_fileDialog->setCaption(i18n("Open Script File"));

    QStringList mimetypes;
    mimetypes << "application/javascript";
    m_fileDialog->setMimeFilter(mimetypes);

    connect(m_fileDialog, SIGNAL(finished(int)), this, SLOT(openScriptUrlSelected(int)));
    m_fileDialog->show();
}

void InteractiveConsole::openScriptUrlSelected(int result)
{
    if (!m_fileDialog) {
        return;
    }

    if (result == QDialog::Accepted) {
        const KUrl url = m_fileDialog->selectedUrl();
        if (!url.isEmpty()) {
            loadScriptFromUrl(url);
        }
    }

    m_fileDialog->deleteLater();
    m_fileDialog = 0;
}

void InteractiveConsole::loadScriptFromUrl(const KUrl &url)
{
    if (m_docEditor) {
        m_docEditor->closeUrl(false);
        m_docEditor->openUrl(url);
        m_docEditor->setHighlightingMode("JavaScript/PlasmaDesktop");
    } else {
        m_editor->clear();
        m_editor->setEnabled(false);

        if (m_job) {
            m_job.data()->kill();
        }

        m_job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);
        connect(m_job.data(), SIGNAL(data(KIO::Job*,QByteArray)), this, SLOT(scriptFileDataRecvd(KIO::Job*,QByteArray)));
        connect(m_job.data(), SIGNAL(result(KJob*)), this, SLOT(reenableEditor(KJob*)));
    }
}

void InteractiveConsole::scriptFileDataRecvd(KIO::Job *job, const QByteArray &data)
{
    Q_ASSERT(m_editor);

    if (job == m_job.data()) {
        m_editor->insertPlainText(data);
    }
}

void InteractiveConsole::saveScript()
{
    if (m_docEditor) {
        m_docEditor->documentSaveAs();
        return;
    }

    delete m_fileDialog;

    m_fileDialog = new KFileDialog(KUrl(), QString(), 0);
    m_fileDialog->setOperationMode(KFileDialog::Saving);
    m_fileDialog->setCaption(i18n("Save Script File"));

    QStringList mimetypes;
    mimetypes << "application/javascript";
    m_fileDialog->setMimeFilter(mimetypes);

    connect(m_fileDialog, SIGNAL(finished(int)), this, SLOT(saveScriptUrlSelected(int)));
    m_fileDialog->show();
}

void InteractiveConsole::saveScriptUrlSelected(int result)
{
    if (!m_fileDialog) {
        return;
    }

    if (result == QDialog::Accepted) {
        const KUrl url = m_fileDialog->selectedUrl();
        if (!url.isEmpty()) {
            saveScript(url);
        }
    }

    m_fileDialog->deleteLater();
    m_fileDialog = 0;
}

void InteractiveConsole::saveScript(const KUrl &url)
{
    if (m_docEditor) {
        m_docEditor->saveAs(url);
    } else {
        m_editor->setEnabled(false);

        if (m_job) {
            m_job.data()->kill();
        }

        m_job = KIO::put(url, -1, KIO::HideProgressInfo);
        connect(m_job.data(), SIGNAL(dataReq(KIO::Job*,QByteArray&)), this, SLOT(scriptFileDataReq(KIO::Job*,QByteArray&)));
        connect(m_job.data(), SIGNAL(result(KJob*)), this, SLOT(reenableEditor(KJob*)));
    }
}

void InteractiveConsole::scriptFileDataReq(KIO::Job *job, QByteArray &data)
{
    Q_ASSERT(m_editor);

    if (!m_job || m_job.data() != job) {
        return;
    }

    data.append(m_editor->toPlainText().toLocal8Bit());
    m_job.clear();
}

void InteractiveConsole::reenableEditor(KJob* job)
{
    Q_ASSERT(m_editor);
    if (m_closeWhenCompleted && job->error() != 0) {
        close();
    }

    m_closeWhenCompleted = false;
    m_editor->setEnabled(true);
}

void InteractiveConsole::evaluateScript()
{
    //kDebug() << "evaluating" << m_editor->toPlainText();
    const QString path = KStandardDirs::locateLocal("appdata", s_autosaveFileName);
    saveScript(path);

    m_output->moveCursor(QTextCursor::End);
    QTextCursor cursor = m_output->textCursor();
    m_output->setTextCursor(cursor);

    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setFontUnderline(true);

    if (cursor.position() > 0) {
        cursor.insertText("\n\n");
    }

    QDateTime dt = QDateTime::currentDateTime();
    cursor.insertText(i18n("Executing script at %1", KGlobal::locale()->formatDateTime(dt)), format);

    format.setFontWeight(QFont::Normal);
    format.setFontUnderline(false);
    QTextBlockFormat block = cursor.blockFormat();
    block.setLeftMargin(10);
    cursor.insertBlock(block, format);
    QElapsedTimer t;
    t.start();

    WorkspaceScripting::DesktopScriptEngine scriptEngine(m_corona, false, this);
    connect(&scriptEngine, SIGNAL(print(QString)), this, SLOT(print(QString)));
    connect(&scriptEngine, SIGNAL(printError(QString)), this, SLOT(print(QString)));
    connect(&scriptEngine, SIGNAL(createPendingPanelViews()), PlasmaApp::self(), SLOT(createWaitingPanels()));
    scriptEngine.evaluateScript(m_docEditor ? m_docEditor->text() : m_editor->toPlainText());

    cursor.insertText("\n\n");
    format.setFontWeight(QFont::Bold);
    // xgettext:no-c-format
    cursor.insertText(i18n("Runtime: %1ms", QString::number(t.elapsed())), format);
    block.setLeftMargin(0);
    cursor.insertBlock(block);
    m_output->ensureCursorVisible();
}

void InteractiveConsole::clearEditor()
{
    if (m_docEditor) {
        m_docEditor->clear();
    } else {
        m_editor->clear();
    }
}

void InteractiveConsole::clearOutput()
{
    m_output->clear();
}

#include "moc_interactiveconsole.cpp"

