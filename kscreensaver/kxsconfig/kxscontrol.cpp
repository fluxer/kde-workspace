//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <KNumInput>

#include <qlabel.h>
#include <qslider.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qxml.h>

//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <klocale.h>
#include <kfiledialog.h>
#include "kxscontrol.h"

//===========================================================================
KXSRangeControl::KXSRangeControl(QWidget *parent, const QString &name,
                                  KConfig &config)
  : QWidget(parent), KXSRangeItem(name, config), mSlider(0), mSpinBox(0)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->addWidget(label);
  mSlider = new QSlider(Qt::Horizontal, this);
  mSlider->setMinimum(mMinimum);
  mSlider->setMaximum(mMaximum);
  mSlider->setPageStep(10);
  mSlider->setValue(mValue);
  connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
  l->addWidget(mSlider);
}

KXSRangeControl::KXSRangeControl(QWidget *parent, const QString &name,
                                  const QXmlAttributes &attr )
  : QWidget(parent), KXSRangeItem(name, attr), mSlider(0), mSpinBox(0)
{
	// NOTE: Contexts used and assembled here have to match with contexts
	// extracted by the PO template creation script.

    if (attr.value(QLatin1String( "type" )) == QLatin1String( "spinbutton" ) ) {
	QHBoxLayout *hb = new QHBoxLayout(this);
	if (!mLabel.isEmpty()) {
	    QLabel *l = new QLabel(i18nc("@label:spinbox", mLabel.toUtf8()), this);
	    hb->addWidget(l);
	}
	mSpinBox = new KIntSpinBox(this);
        mSpinBox->setMinimum(mMinimum);
        mSpinBox->setMaximum(mMaximum);
        mSpinBox->setSingleStep(1);
	mSpinBox->setValue(mValue);
	connect(mSpinBox, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
	hb->addWidget(mSpinBox);
    } else {
	QString lowLabel = attr.value(QLatin1String( "_low-label" ));
	QString highLabel = attr.value(QLatin1String( "_high-label" ));
	QVBoxLayout *vb = new QVBoxLayout(this);
	if (!mLabel.isEmpty()) {
	    QLabel *l = new QLabel(i18nc("@label:slider", mLabel.toUtf8()), this);
	    vb->addWidget(l);
	}
	QHBoxLayout *hb = new QHBoxLayout();
        vb->addLayout(hb);
	QString limCtxt = mLabel.isEmpty() ? QLatin1String("@item:inrange")
	                                   : QString(QLatin1String( "@item:inrange %1" )).arg(mLabel);
	if (!lowLabel.isEmpty()) {
	    QLabel *l = new QLabel(i18nc(limCtxt.toUtf8(), lowLabel.toUtf8()), this);
	    hb->addWidget(l);
	}
	mSlider = new QSlider(Qt::Horizontal, this);
	mSlider->setMinimum(mMinimum);
	mSlider->setMaximum(mMaximum);
	mSlider->setPageStep(10);
	mSlider->setValue(mValue);
	connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
	hb->addWidget(mSlider);
	if (!highLabel.isEmpty()){
	    QLabel *l = new QLabel(i18nc(limCtxt.toUtf8(), highLabel.toUtf8()), this);
	    hb->addWidget(l);
	}
    }
}

void KXSRangeControl::slotValueChanged(int value)
{
  mValue = value;
  emit changed();
}

void KXSRangeControl::read(KConfig &config)
{
    KXSRangeItem::read(config);
    if ( mSpinBox )
	mSpinBox->setValue(mValue);
    else
	mSlider->setValue(mValue);
}

//===========================================================================
KXSDoubleRangeControl::KXSDoubleRangeControl(QWidget *parent,
                                  const QString &name, KConfig &config)
  : QWidget(parent), KXSDoubleRangeItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->addWidget(label);

  int value = int((mValue - mMinimum) * 100 / (mMaximum - mMinimum));

  mSlider = new QSlider(Qt::Horizontal, this);
  mSlider->setMinimum(0);
  mSlider->setMaximum(100);
  mSlider->setPageStep(10);
  mSlider->setValue(value);
  connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
  l->addWidget(mSlider);
}

KXSDoubleRangeControl::KXSDoubleRangeControl(QWidget *parent,
                                  const QString &name, const QXmlAttributes &attr)
  : QWidget(parent), KXSDoubleRangeItem(name, attr)
{
    // NOTE: Contexts used and assembled here have to match with contexts
    // extracted by the PO template creation script.

    QString lowLabel = attr.value(QLatin1String( "_low-label" ));
    QString highLabel = attr.value(QLatin1String( "_high-label" ));
    QVBoxLayout *vb = new QVBoxLayout(this);
    if (!mLabel.isEmpty()) {
	QLabel *l = new QLabel(i18nc("@label:slider", mLabel.toUtf8()), this);
	vb->addWidget(l);
    }
    QHBoxLayout *hb = new QHBoxLayout();
    vb->addLayout(hb);
    QString limCtxt = !mLabel.isEmpty() ? QString(QLatin1String( "@item:inrange %1" )).arg(mLabel)
                                        : QLatin1String("@item:inrange");
    if (!lowLabel.isEmpty()) {
	QLabel *l = new QLabel(i18nc(limCtxt.toUtf8(), lowLabel.toUtf8()), this);
	hb->addWidget(l);
    }
    int value = int((mValue - mMinimum) * 100 / (mMaximum - mMinimum));
    mSlider = new QSlider(Qt::Horizontal, this);
    mSlider->setMinimum(0);
    mSlider->setMaximum(100);
    mSlider->setPageStep(10);
    mSlider->setValue(value);
    connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
    hb->addWidget(mSlider);
    if (!highLabel.isEmpty()){
	QLabel *l = new QLabel(i18nc(limCtxt.toUtf8(), highLabel.toUtf8()), this);
	hb->addWidget(l);
    }
}

void KXSDoubleRangeControl::slotValueChanged(int value)
{
  mValue = mMinimum + value * (mMaximum - mMinimum) / 100.0;
  emit changed();
}

void KXSDoubleRangeControl::read(KConfig &config)
{
    KXSDoubleRangeItem::read(config);
    mSlider->setValue((int)((mValue - mMinimum) * 100.0 /
                            (mMaximum - mMinimum) + 0.5));
}

//===========================================================================
KXSCheckBoxControl::KXSCheckBoxControl(QWidget *parent, const QString &name,
                                      KConfig &config)
  : QCheckBox(parent), KXSBoolItem(name, config)
{
  setText(mLabel);
  setChecked(mValue);
  connect(this, SIGNAL(toggled(bool)), SLOT(slotToggled(bool)));
}

KXSCheckBoxControl::KXSCheckBoxControl(QWidget *parent, const QString &name,
                                      const QXmlAttributes &attr)
  : QCheckBox(parent), KXSBoolItem(name, attr)
{
  // NOTE: Contexts used and assembled here have to match with contexts
  // extracted by the PO template creation script.
  setText(i18nc("@option:check", mLabel.toUtf8()));
  setChecked(mValue);
  connect(this, SIGNAL(toggled(bool)), SLOT(slotToggled(bool)));
}

void KXSCheckBoxControl::slotToggled(bool state)
{
    mValue = state;
    emit changed();
}

void KXSCheckBoxControl::read(KConfig &config)
{
    KXSBoolItem::read(config);
    setChecked(mValue);
}

//===========================================================================
KXSDropListControl::KXSDropListControl(QWidget *parent, const QString &name,
                                      KConfig &config)
  : QWidget(parent), KXSSelectItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->addWidget(label);
  mCombo = new QComboBox(this);
  for(int i=0; i < mOptions.count(); i++)
      mCombo->addItem( i18n(mOptions[i].toUtf8()) );
  mCombo->setCurrentIndex(mValue);
  connect(mCombo, SIGNAL(activated(int)), SLOT(slotActivated(int)));
  l->addWidget(mCombo);
}

KXSDropListControl::KXSDropListControl(QWidget *parent, const QString &name,
                                      const QXmlAttributes &attr)
  : QWidget(parent), KXSSelectItem(name, attr)
{
  // NOTE: Contexts used and assembled here have to match with contexts
  // extracted by the PO template creation script.

  QVBoxLayout *l = new QVBoxLayout(this);
  QString labelText = mLabel.isEmpty() ? QString():i18nc("@title:group", mLabel.toUtf8());
  QLabel *label = new QLabel(labelText, this);
  l->addWidget(label);
  mCombo = new QComboBox(this);
  connect(mCombo, SIGNAL(activated(int)), SLOT(slotActivated(int)));
  l->addWidget(mCombo);
}

void KXSDropListControl::addOption(const QXmlAttributes &attr)
{
    // NOTE: Contexts used and assembled here have to match with contexts
    // extracted by the PO template creation script.

    QString itemCtxt = mLabel.isEmpty() ? QLatin1String("@option:radio")
                                        : QString(QLatin1String( "@option:radio %1" )).arg(mLabel);
    KXSSelectItem::addOption( attr );
    mCombo->addItem( i18nc(itemCtxt.toUtf8(), mOptions[mOptions.count()-1].toUtf8()) );
    if ( mValue == mOptions.count()-1 )
	mCombo->setCurrentIndex(mOptions.count()-1);
}

void KXSDropListControl::slotActivated(int indx)
{
  mValue = indx;
  emit changed();
}

void KXSDropListControl::read(KConfig &config)
{
    KXSSelectItem::read(config);
    mCombo->setCurrentIndex(mValue);
}

//===========================================================================
KXSLineEditControl::KXSLineEditControl(QWidget *parent, const QString &name,
                                  KConfig &config)
  : QWidget(parent), KXSStringItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->addWidget(label);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
  l->addWidget(mEdit);
}

KXSLineEditControl::KXSLineEditControl(QWidget *parent, const QString &name,
                                  const QXmlAttributes &attr )
  : QWidget(parent), KXSStringItem(name, attr)
{
  // NOTE: Contexts used and assembled here have to match with contexts
  // extracted by the PO template creation script.

  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(i18nc("@label:textbox", mLabel.toUtf8()), this);
  l->addWidget(label);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
  l->addWidget(mEdit);
}

void KXSLineEditControl::textChanged( const QString &text )
{
    mValue = text;
    emit changed();
}

void KXSLineEditControl::read(KConfig &config)
{
    KXSStringItem::read(config);
    mEdit->setText(mValue);
}

//===========================================================================
KXSFileControl::KXSFileControl(QWidget *parent, const QString &name,
                                  KConfig &config)
  : QWidget(parent), KXSStringItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->addWidget(label);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
  l->addWidget(mEdit);
}

KXSFileControl::KXSFileControl(QWidget *parent, const QString &name,
                                  const QXmlAttributes &attr )
  : QWidget(parent), KXSStringItem(name, attr)
{
  // NOTE: Contexts used and assembled here have to match with contexts
  // extracted by the PO template creation script.

  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(i18nc("@label:chooser", mLabel.toUtf8()), this);
  l->addWidget(label);
  QHBoxLayout *hb = new QHBoxLayout();
  l->addLayout(hb);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
  hb->addWidget(mEdit);
  QPushButton *pb = new QPushButton( QLatin1String( "..." ), this );
  connect( pb, SIGNAL(clicked()), this, SLOT(selectFile()) );
  hb->addWidget(pb);
}

void KXSFileControl::textChanged( const QString &text )
{
    mValue = text;
    emit changed();
}

void KXSFileControl::selectFile()
{
    QString f = KFileDialog::getOpenFileName();
    if ( !f.isEmpty() ) {
	mValue = f;
	mEdit->setText(mValue);
	emit changed();
    }
}

void KXSFileControl::read(KConfig &config)
{
    KXSStringItem::read(config);
    mEdit->setText(mValue);
}

#include "kxscontrol.moc"
