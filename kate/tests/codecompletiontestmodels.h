/* This file is part of the KDE libraries
   Copyright (C) 2008 Niko Sams <niko.sams\gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KATE_COMPLETIONTESTMODELS_H
#define KATE_COMPLETIONTESTMODELS_H

#include "codecompletiontestmodel.h"
#include <ktexteditor/codecompletionmodelcontrollerinterface.h>

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

class CustomRangeModel : public CodeCompletionTestModel, public KTextEditor::CodeCompletionModelControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)
public:
    CustomRangeModel(KTextEditor::View* parent = 0L, const QString &startText = QString())
        : CodeCompletionTestModel(parent, startText)
    {}
    KTextEditor::Range completionRange(KTextEditor::View* view, const KTextEditor::Cursor &position)
    {
        KTextEditor::Range range = CodeCompletionModelControllerInterface::completionRange(view, position);
        if (range.start().column() > 0) {
            KTextEditor::Range preRange(KTextEditor::Cursor(range.start().line(), range.start().column()-1),
                                        KTextEditor::Cursor(range.start().line(), range.start().column()));
            kDebug() << preRange << view->document()->text(preRange);
            if (view->document()->text(preRange) == "$") {
                range.expandToRange(preRange);
                kDebug() << "using custom completion range" << range;
            }
        }
        return range;
    }

    bool shouldAbortCompletion(KTextEditor::View* view, const KTextEditor::Range& range, const QString &currentCompletion)
    {
        Q_UNUSED(view);
        Q_UNUSED(range);
        static const QRegExp allowedText("^\\$?(\\w*)");
        return !allowedText.exactMatch(currentCompletion);
    }
};

class CustomAbortModel : public CodeCompletionTestModel, public KTextEditor::CodeCompletionModelControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)
public:
    CustomAbortModel(KTextEditor::View* parent = 0L, const QString &startText = QString())
        : CodeCompletionTestModel(parent, startText)
    {}

    bool shouldAbortCompletion(KTextEditor::View* view, const KTextEditor::Range& range, const QString &currentCompletion)
    {
        Q_UNUSED(view);
        Q_UNUSED(range);
        static const QRegExp allowedText("^([\\w-]*)");
        return !allowedText.exactMatch(currentCompletion);
    }
};

class EmptyFilterStringModel : public CodeCompletionTestModel, public KTextEditor::CodeCompletionModelControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)
public:
    EmptyFilterStringModel(KTextEditor::View* parent = 0L, const QString &startText = QString())
        : CodeCompletionTestModel(parent, startText)
    {}

    QString filterString(KTextEditor::View*, const KTextEditor::Range&, const KTextEditor::Cursor &)
    {
        return QString();
    }
};

class UpdateCompletionRangeModel : public CodeCompletionTestModel, public KTextEditor::CodeCompletionModelControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)
public:
    UpdateCompletionRangeModel(KTextEditor::View* parent = 0L, const QString &startText = QString())
        : CodeCompletionTestModel(parent, startText)
    {}

    KTextEditor::Range updateCompletionRange(KTextEditor::View* view, const KTextEditor::Range& range)
    {
        Q_UNUSED(view);
        if (view->document()->text(range) == QString("ab")) {
            return KTextEditor::Range(KTextEditor::Cursor(range.start().line(), 0), range.end());
        }
        return range;
    }
    bool shouldAbortCompletion(KTextEditor::View* view, const KTextEditor::Range &range, const QString &currentCompletion)
    {
        Q_UNUSED(view);
        Q_UNUSED(range);
        Q_UNUSED(currentCompletion);
        return false;
    }
};

class StartCompletionModel : public CodeCompletionTestModel, public KTextEditor::CodeCompletionModelControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)
public:
    StartCompletionModel(KTextEditor::View* parent = 0L, const QString &startText = QString())
        : CodeCompletionTestModel(parent, startText)
    {}

    bool shouldStartCompletion(KTextEditor::View* view, const QString &insertedText, bool userInsertion, const KTextEditor::Cursor &position)
    {
        Q_UNUSED(view);
        Q_UNUSED(userInsertion);
        Q_UNUSED(position);
        if(insertedText.isEmpty())
            return false;

        QChar lastChar = insertedText.at(insertedText.count() - 1);
        if (lastChar == '%') {
            return true;
        }
        return false;
    }
};

class ImmideatelyAbortCompletionModel : public CodeCompletionTestModel, public KTextEditor::CodeCompletionModelControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(KTextEditor::CodeCompletionModelControllerInterface)
public:
    ImmideatelyAbortCompletionModel(KTextEditor::View* parent = 0L, const QString &startText = QString())
        : CodeCompletionTestModel(parent, startText)
    {}

    virtual bool shouldAbortCompletion(KTextEditor::View* view, const KTextEditor::Range& range, const QString& currentCompletion)
    {
        Q_UNUSED(view);
        Q_UNUSED(range);
        Q_UNUSED(currentCompletion);
        return true;
    }
};

#endif
