/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

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

#include "keyboardconfig.h"

#include <QHeaderView>
#include <QX11Info>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <kkeyboardlayout.h>
#include <kstandarddirs.h>
#include <kicon.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdialog.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

static const float s_defaultrepeatdelay = 660.0;
static const float s_defaultrepeatrate = 25.0;

static QList<KKeyboardType> kLayoutsFromConfig()
{
    KConfig kconfig("kcminputrc", KConfig::NoGlobals);
    KConfigGroup kconfiggroup(&kconfig, "Keyboard");
    const KKeyboardType defaultlayout = KKeyboardLayout::defaultLayout();
    const QByteArray layoutsmodel = kconfiggroup.readEntry("LayoutsModel", defaultlayout.model);
    const QStringList layoutslayouts = kconfiggroup.readEntry(
        "LayoutsLayouts",
        QStringList() << QString::fromLatin1(defaultlayout.layout.constData(), defaultlayout.layout.size())
    );
    const QStringList layoutsvariants = kconfiggroup.readEntry(
        "LayoutsVariants",
        QStringList() << QString::fromLatin1(defaultlayout.variant.constData(), defaultlayout.variant.size())
    );
    QList<KKeyboardType> result;
    for (int i = 0; i < layoutslayouts.size(); i++) {
        KKeyboardType kkeyboardtype;
        kkeyboardtype.model = layoutsmodel;
        kkeyboardtype.layout = layoutslayouts.at(i).toLatin1();
        if (i < layoutsvariants.size()) {
            kkeyboardtype.variant = layoutsvariants.at(i).toLatin1();
        }
        result.append(kkeyboardtype);
    }
    return result;
}

static void kFillTreeFromLayouts(QTreeWidget *treewidget, const QList<KKeyboardType> &layouts)
{
    treewidget->clear();
    int itemcounter = 0;
    foreach (const KKeyboardType &layout, layouts) {
        QTreeWidgetItem* layoutitem = new QTreeWidgetItem();
        layoutitem->setData(0, Qt::UserRole, layout.layout);
        layoutitem->setData(0, Qt::UserRole + 1, itemcounter);
        layoutitem->setText(0, KKeyboardLayout::layoutDescription(layout.layout));
        layoutitem->setData(1, Qt::UserRole, layout.variant);
        layoutitem->setText(1, KKeyboardLayout::variantDescription(layout.layout, layout.variant));
        treewidget->addTopLevelItem(layoutitem);
        itemcounter++;
    }
}

static QList<KKeyboardType> kGetLayoutsFromTree(QTreeWidget *treewidget, const QByteArray &model)
{
    QList<KKeyboardType> result;
    for (int i = 0; i < treewidget->topLevelItemCount(); i++) {
        QTreeWidgetItem* layoutitem = treewidget->topLevelItem(i);
        KKeyboardType kkeyboardtype;
        kkeyboardtype.model = model;
        kkeyboardtype.layout = layoutitem->data(0, Qt::UserRole).toByteArray();
        kkeyboardtype.variant = layoutitem->data(1, Qt::UserRole).toByteArray();
        result.append(kkeyboardtype);
    }
    return result;
}

static void kApplyKeyboardConfig()
{
    const QList<KKeyboardType> layouts = kLayoutsFromConfig();
    if (layouts.size() > 0) {
        KKeyboardLayout().setLayouts(layouts);
    }

    KConfig kconfig("kcminputrc", KConfig::NoGlobals);
    KConfigGroup kconfiggroup(&kconfig, "Keyboard");
    const float repeatdelay = kconfiggroup.readEntry("RepeatDelay", s_defaultrepeatdelay);
    const float repeatrate = kconfiggroup.readEntry("RepeatRate", s_defaultrepeatrate);

    XkbDescPtr xkbkeyboard = XkbAllocKeyboard();
    if (!xkbkeyboard) {
        kError() << "Failed to allocate keyboard";
        return;
    }
    Status xkbgetresult = XkbGetControls(QX11Info::display(), XkbRepeatKeysMask, xkbkeyboard);
    if (xkbgetresult != Success) {
        kError() << "Failed to get keyboard repeat controls";
        XkbFreeKeyboard(xkbkeyboard, 0, true);
        return;
    }
    xkbkeyboard->ctrls->repeat_delay = repeatdelay;
    xkbkeyboard->ctrls->repeat_interval = qFloor(1000 / repeatrate + 0.5);
    const Bool xkbsetresult = XkbSetControls(QX11Info::display(), XkbRepeatKeysMask, xkbkeyboard);
    if (xkbsetresult != True) {
        kError() << "Failed to set keyboard repeat controls";
    }
    XkbFreeKeyboard(xkbkeyboard, 0, true);
}

class KCMKeyboardDialog : public KDialog
{
    Q_OBJECT
public:
    KCMKeyboardDialog(QWidget *parent);
    ~KCMKeyboardDialog();

    KKeyboardType keyboardType() const;
    void setKeyboardType(const KKeyboardType &layout);

Q_SIGNALS:
    void save();

private Q_SLOTS:
    void slotLayoutIndexChanged(const int index);
    void slotVariantIndexChanged(const int index);

private:
    KKeyboardType m_keyboardtype;
    QWidget* m_widget;
    QGridLayout* m_layout;
    QLabel* m_layoutslabel;
    QComboBox* m_layoutsbox;
    QLabel* m_variantslabel;
    QComboBox* m_variantsbox;
};

KCMKeyboardDialog::KCMKeyboardDialog(QWidget *parent)
    : KDialog(parent),
    m_keyboardtype(KKeyboardLayout::defaultLayout()),
    m_widget(nullptr),
    m_layout(nullptr),
    m_layoutslabel(nullptr),
    m_layoutsbox(nullptr),
    m_variantslabel(nullptr),
    m_variantsbox(nullptr)
{
    setCaption(i18n("Keyboard Layout"));
    setButtons(KDialog::Ok | KDialog::Cancel);

    m_widget = new QWidget(this);
    m_layout = new QGridLayout(m_widget);

    // TODO: do not add layouts and variants already in the tree
    m_layoutslabel = new QLabel(m_widget);
    m_layoutslabel->setText(i18n("Layout:"));
    m_layout->addWidget(m_layoutslabel, 0, 0);
    m_layoutsbox = new QComboBox(m_widget);
    m_layoutsbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    foreach (const QByteArray &layoutlayout, KKeyboardLayout::layoutNames()) {
        m_layoutsbox->addItem(KKeyboardLayout::layoutDescription(layoutlayout), layoutlayout);
    }
    connect(
        m_layoutsbox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotLayoutIndexChanged(int))
    );
    m_layout->addWidget(m_layoutsbox, 0, 1);

    m_variantslabel = new QLabel(m_widget);
    m_variantslabel->setText(i18n("Variant:"));
    m_layout->addWidget(m_variantslabel, 1, 0);
    m_variantsbox = new QComboBox(m_widget);
    m_variantsbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(
        m_variantsbox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotVariantIndexChanged(int))
    );
    m_layout->addWidget(m_variantsbox, 1, 1);

    setMainWidget(m_widget);

    setInitialSize(QSize(100, 200));
    KConfigGroup kconfiggroup(KGlobal::config(), "KCMKeyboardDialog");
    restoreDialogSize(kconfiggroup);
}

KCMKeyboardDialog::~KCMKeyboardDialog()
{
    KConfigGroup kconfiggroup(KGlobal::config(), "KCMKeyboardDialog");
    saveDialogSize(kconfiggroup);
    KGlobal::config()->sync();
}

KKeyboardType KCMKeyboardDialog::keyboardType() const
{
    return m_keyboardtype;
}

void KCMKeyboardDialog::setKeyboardType(const KKeyboardType &layout)
{
    m_keyboardtype = layout;
    m_variantsbox->blockSignals(true);
    const int layoutindex = m_layoutsbox->findData(m_keyboardtype.layout);
    if (layoutindex >= 0) {
        m_layoutsbox->setCurrentIndex(layoutindex);
    } else {
        kWarning() << "Could not find the keyboard layout" << m_keyboardtype.layout;
    }
    const int variantindex = m_variantsbox->findData(m_keyboardtype.variant);
    if (variantindex >= 0) {
        m_variantsbox->setCurrentIndex(variantindex);
    } else {
        kWarning() << "Could not find the keyboard variant" << m_keyboardtype.variant;
    }
    m_variantsbox->blockSignals(false);
}

void KCMKeyboardDialog::slotLayoutIndexChanged(const int index)
{
    m_variantsbox->clear();
    const QByteArray layoutlayout = m_layoutsbox->itemData(index).toByteArray();
    // add "None"
    m_variantsbox->addItem(KKeyboardLayout::variantDescription(layoutlayout, QByteArray()), QByteArray());
    foreach (const QByteArray &layoutvariant, KKeyboardLayout::variantNames(layoutlayout)) {
        m_variantsbox->addItem(KKeyboardLayout::variantDescription(layoutlayout, layoutvariant), layoutvariant);
    }
    m_keyboardtype.layout = layoutlayout;
}

void KCMKeyboardDialog::slotVariantIndexChanged(const int index)
{
    const QByteArray layoutvariant = m_variantsbox->itemData(index).toByteArray();
    m_keyboardtype.variant = layoutvariant;
}


extern "C"
{
    Q_DECL_EXPORT void kcminit_keyboard()
    {
        kApplyKeyboardConfig();
    }
}

K_PLUGIN_FACTORY(KCMKeyboardFactory, registerPlugin<KCMKeyboard>();)
K_EXPORT_PLUGIN(KCMKeyboardFactory("kcmkeyboard", "kcm_keyboard"))

KCMKeyboard::KCMKeyboard(QWidget *parent, const QVariantList &args)
    : KCModule(KCMKeyboardFactory::componentData(), parent),
    m_layout(nullptr),
    m_repeatgroup(nullptr),
    m_repeatdelaylabel(nullptr),
    m_repeatdelayinput(nullptr),
    m_repeatratelabel(nullptr),
    m_repeatrateinput(nullptr),
    m_layoutsgroup(nullptr),
    m_layoutsmodellabel(nullptr),
    m_layoutsmodelbox(nullptr),
    m_layoutstree(nullptr),
    m_layoutbuttonsbox(nullptr),
    m_layoutsbuttonsspacer(nullptr),
    m_layoutsaddbutton(nullptr),
    m_layoutseditbutton(nullptr),
    m_layoutsremovebutton(nullptr),
    m_layoutsupbutton(nullptr),
    m_layoutsdownbutton(nullptr),
    m_layoutsbuttonsspacer2(nullptr)
{
    Q_UNUSED(args);

    KGlobal::locale()->insertCatalog("kxkb");

    setButtons(KCModule::Default | KCModule::Apply);
    setQuickHelp(i18n("<h1>Keyboard</h1> This control module can be used to configure keyboard parameters and layouts."));

    KAboutData *about = new KAboutData(
        I18N_NOOP("kcmkeyboard"), 0,
        ki18n("KDE Keyboard Control Module"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("Copyright 2023, Ivailo Monev <email>xakepa10@gmail.com</email>")
    );
    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    m_layout = new QVBoxLayout(this);
    setLayout(m_layout);

    m_repeatgroup = new QGroupBox(this);
    m_repeatgroup->setTitle(i18n("Repeat"));
    QGridLayout* repeatgrouplayout = new QGridLayout(m_repeatgroup);
    m_repeatgroup->setLayout(repeatgrouplayout);
    m_layout->addWidget(m_repeatgroup);

    m_repeatdelaylabel = new QLabel(m_repeatgroup);
    m_repeatdelaylabel->setText(i18n("Repeat delay:"));
    repeatgrouplayout->addWidget(m_repeatdelaylabel, 0, 0);
    m_repeatdelayinput = new KIntNumInput(m_repeatgroup);
    m_repeatdelayinput->setSliderEnabled(true);
    m_repeatdelayinput->setSuffix(i18n(" ms"));
    const QString repeatdelayhelp = i18n("The delay after which a pressed key will start generating keycodes.");
    m_repeatdelayinput->setToolTip(repeatdelayhelp);
    m_repeatdelayinput->setWhatsThis(repeatdelayhelp);
    m_repeatdelayinput->setRange(10, 10000);
    m_repeatdelayinput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(
        m_repeatdelayinput, SIGNAL(valueChanged(int)),
        this, SLOT(slotEmitChanged())
    );
    repeatgrouplayout->addWidget(m_repeatdelayinput, 0, 1);

    m_repeatratelabel = new QLabel(m_repeatgroup);
    m_repeatratelabel->setText(i18n("Repeat rate:"));
    repeatgrouplayout->addWidget(m_repeatratelabel, 1, 0);
    m_repeatrateinput = new KIntNumInput(m_repeatgroup);
    m_repeatrateinput->setSliderEnabled(true);
    m_repeatrateinput->setSuffix(i18n(" ms"));
    const QString repeatratehelp = i18n("The rate at which keycodes are generated while a key is pressed.");
    m_repeatrateinput->setToolTip(repeatratehelp);
    m_repeatrateinput->setWhatsThis(repeatratehelp);
    m_repeatrateinput->setRange(10, 5000);
    m_repeatrateinput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(
        m_repeatrateinput, SIGNAL(valueChanged(int)),
        this, SLOT(slotEmitChanged())
    );
    repeatgrouplayout->addWidget(m_repeatrateinput, 1, 1);

    m_layoutsgroup = new QGroupBox(this);
    m_layoutsgroup->setTitle(i18n("Layouts"));
    QGridLayout* layoutgrouplayout = new QGridLayout(m_layoutsgroup);
    m_layoutsgroup->setLayout(layoutgrouplayout);
    m_layout->addWidget(m_layoutsgroup);

    m_layoutsmodellabel = new QLabel(m_layoutsgroup);
    m_layoutsmodellabel->setText(i18n("Model:"));
    layoutgrouplayout->addWidget(m_layoutsmodellabel, 0, 0);
    m_layoutsmodelbox = new QComboBox(m_layoutsgroup);
    m_layoutsmodelbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    foreach (const QByteArray &layoutmodel, KKeyboardLayout::modelNames()) {
        m_layoutsmodelbox->addItem(KKeyboardLayout::modelDescription(layoutmodel), layoutmodel);
    }
    connect(
        m_layoutsmodelbox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotEmitChanged())
    );
    layoutgrouplayout->addWidget(m_layoutsmodelbox, 0, 1);

    m_layoutstree = new QTreeWidget(m_layoutsgroup);
    m_layoutstree->setColumnCount(2);
    QStringList treeheaders = QStringList()
        << i18n("Layout")
        << i18n("Variant");
    m_layoutstree->setHeaderLabels(treeheaders);
    m_layoutstree->setRootIsDecorated(false);
    m_layoutstree->header()->setStretchLastSection(false);
    m_layoutstree->header()->setResizeMode(0, QHeaderView::Stretch);
    m_layoutstree->header()->setResizeMode(1, QHeaderView::Stretch);
    connect(
        m_layoutstree, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this, SLOT(slotEmitChanged())
    );
    connect(
        m_layoutstree, SIGNAL(itemSelectionChanged()),
        this, SLOT(slotItemSelectionChanged())
    );
    layoutgrouplayout->addWidget(m_layoutstree, 1, 0, 1, 2);

    m_layoutbuttonsbox = new KHBox(m_layoutsgroup);
    layoutgrouplayout->addWidget(m_layoutbuttonsbox, 2, 0, 1, 2);

    m_layoutsbuttonsspacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layoutbuttonsbox->layout()->addItem(m_layoutsbuttonsspacer);

    m_layoutsaddbutton = new QPushButton(m_layoutbuttonsbox);
    m_layoutsaddbutton->setText(i18n("Add"));
    m_layoutsaddbutton->setIcon(KIcon("list-add"));
    connect(
        m_layoutsaddbutton, SIGNAL(pressed()),
        this, SLOT(slotAddPressed())
    );

    m_layoutseditbutton = new QPushButton(m_layoutbuttonsbox);
    m_layoutseditbutton->setText(i18n("Edit"));
    m_layoutseditbutton->setIcon(KIcon("document-edit"));
    m_layoutseditbutton->setEnabled(false);
    connect(
        m_layoutseditbutton, SIGNAL(pressed()),
        this, SLOT(slotEditPressed())
    );

    m_layoutsremovebutton = new QPushButton(m_layoutbuttonsbox);
    m_layoutsremovebutton->setText(i18n("Remove"));
    m_layoutsremovebutton->setIcon(KIcon("list-remove"));
    m_layoutsremovebutton->setEnabled(false);
    connect(
        m_layoutsremovebutton, SIGNAL(pressed()),
        this, SLOT(slotRemovePressed())
    );

    m_layoutsupbutton = new QPushButton(m_layoutbuttonsbox);
    m_layoutsupbutton->setText(i18n("Move Up"));
    m_layoutsupbutton->setIcon(KIcon("go-up"));
    m_layoutsupbutton->setEnabled(false);
    connect(
        m_layoutsupbutton, SIGNAL(pressed()),
        this, SLOT(slotUpPressed())
    );

    m_layoutsdownbutton = new QPushButton(m_layoutbuttonsbox);
    m_layoutsdownbutton->setText(i18n("Move Down"));
    m_layoutsdownbutton->setIcon(KIcon("go-down"));
    m_layoutsdownbutton->setEnabled(false);
    connect(
        m_layoutsdownbutton, SIGNAL(pressed()),
        this, SLOT(slotDownPressed())
    );

    m_layoutsbuttonsspacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layoutbuttonsbox->layout()->addItem(m_layoutsbuttonsspacer2);
}

KCMKeyboard::~KCMKeyboard()
{
}

void KCMKeyboard::load()
{
    KConfig kconfig("kcminputrc", KConfig::NoGlobals);
    KConfigGroup kconfiggroup(&kconfig, "Keyboard");
    const float repeatdelay = kconfiggroup.readEntry("RepeatDelay", s_defaultrepeatdelay);
    const float repeatrate = kconfiggroup.readEntry("RepeatRate", s_defaultrepeatrate);
    m_repeatdelayinput->setValue(qRound(repeatdelay));
    m_repeatrateinput->setValue(qRound(repeatrate));

    const QList<KKeyboardType> layouts = kLayoutsFromConfig();
    const KKeyboardType modellayout = (layouts.size() > 0 ? layouts.first() : KKeyboardLayout::defaultLayout());
    m_layoutmodel = modellayout.model;
    const int layoutsmodelindex = m_layoutsmodelbox->findData(m_layoutmodel);
    if (layoutsmodelindex >= 0) {
        m_layoutsmodelbox->setCurrentIndex(layoutsmodelindex);
    } else {
        kWarning() << "Could not find the keyboard layout model" << m_layoutmodel;
    }

    kFillTreeFromLayouts(m_layoutstree, layouts);

    emit changed(false);
}

void KCMKeyboard::save()
{
    KConfig kconfig("kcminputrc", KConfig::NoGlobals);
    KConfigGroup kconfiggroup(&kconfig, "Keyboard");
    kconfiggroup.writeEntry("RepeatDelay", m_repeatdelayinput->value());
    kconfiggroup.writeEntry("RepeatRate", m_repeatrateinput->value());
    m_layoutmodel = m_layoutsmodelbox->itemData(m_layoutsmodelbox->currentIndex()).toByteArray();
    QStringList layoutslayouts;
    QStringList layoutsvariants;
    for (int i = 0; i < m_layoutstree->topLevelItemCount(); i++) {
        QTreeWidgetItem* layoutitem = m_layoutstree->topLevelItem(i);
        layoutslayouts.append(layoutitem->data(0, Qt::UserRole).toString());
        layoutsvariants.append(layoutitem->data(1, Qt::UserRole).toString());
    }
    kconfiggroup.writeEntry("LayoutsModel", m_layoutmodel);
    kconfiggroup.writeEntry("LayoutsLayouts", layoutslayouts);
    kconfiggroup.writeEntry("LayoutsVariants", layoutsvariants);
    kconfig.sync();

    kApplyKeyboardConfig();

    emit changed(false);
}

void KCMKeyboard::defaults()
{
    m_repeatdelayinput->setValue(qRound(s_defaultrepeatdelay));
    m_repeatrateinput->setValue(qRound(s_defaultrepeatrate));

    const KKeyboardType defaultlayout = KKeyboardLayout::defaultLayout();
    const int defaultmodelindex = m_layoutsmodelbox->findData(defaultlayout.model);
    if (defaultmodelindex >= 0) {
        m_layoutsmodelbox->setCurrentIndex(defaultmodelindex);
    } else {
        kWarning() << "Could not find the default keyboard layout model" << defaultlayout.model;
    }

    const QList<KKeyboardType> layouts = QList<KKeyboardType>()
        << defaultlayout;
    kFillTreeFromLayouts(m_layoutstree, layouts);

    emit changed(true);
}

void KCMKeyboard::slotEmitChanged()
{
    emit changed(true);
}

void KCMKeyboard::slotItemSelectionChanged()
{
    QList<QTreeWidgetItem*> selectedlayouts = m_layoutstree->selectedItems();
    if (selectedlayouts.size() == 0) {
        m_layoutseditbutton->setEnabled(false);
        m_layoutsremovebutton->setEnabled(false);
        m_layoutsupbutton->setEnabled(false);
        m_layoutsdownbutton->setEnabled(false);
        return;
    }

    m_layoutseditbutton->setEnabled(true);
    m_layoutsremovebutton->setEnabled(true);
    if (m_layoutstree->topLevelItemCount() == 1) {
        m_layoutsupbutton->setEnabled(false);
        m_layoutsdownbutton->setEnabled(false);
        return;
    }

    const int selectedrow = selectedlayouts.at(0)->data(0, Qt::UserRole + 1).toInt();
    m_layoutsupbutton->setEnabled(selectedrow > 0);
    m_layoutsdownbutton->setEnabled((selectedrow + 1) < m_layoutstree->topLevelItemCount());
}

void KCMKeyboard::slotAddPressed()
{
    KCMKeyboardDialog keyboarddialog(this);
    keyboarddialog.setKeyboardType(KKeyboardLayout::defaultLayout());
    if (keyboarddialog.exec() != QDialog::Accepted) {
        return;
    }
    QList<KKeyboardType> layouts = kGetLayoutsFromTree(m_layoutstree, m_layoutmodel);
    layouts.append(keyboarddialog.keyboardType());
    kFillTreeFromLayouts(m_layoutstree, layouts);
    emit changed(true);
}

void KCMKeyboard::slotEditPressed()
{
    QList<QTreeWidgetItem*> selectedlayouts = m_layoutstree->selectedItems();
    if (selectedlayouts.size() == 0) {
        return;
    }
    const int selectedrow = selectedlayouts.at(0)->data(0, Qt::UserRole + 1).toInt();
    QList<KKeyboardType> layouts = kGetLayoutsFromTree(m_layoutstree, m_layoutmodel);
    KCMKeyboardDialog keyboarddialog(this);
    keyboarddialog.setKeyboardType(layouts.at(selectedrow));
    if (keyboarddialog.exec() != QDialog::Accepted) {
        return;
    }
    layouts.removeAt(selectedrow);
    layouts.insert(selectedrow, keyboarddialog.keyboardType());
    kFillTreeFromLayouts(m_layoutstree, layouts);
    emit changed(true);
}

void KCMKeyboard::slotRemovePressed()
{
    QList<QTreeWidgetItem*> selectedlayouts = m_layoutstree->selectedItems();
    if (selectedlayouts.size() == 0) {
        return;
    }
    const int selectedrow = selectedlayouts.at(0)->data(0, Qt::UserRole + 1).toInt();
    QList<KKeyboardType> layouts = kGetLayoutsFromTree(m_layoutstree, m_layoutmodel);
    layouts.removeAt(selectedrow);
    kFillTreeFromLayouts(m_layoutstree, layouts);
    emit changed(true);
}

void KCMKeyboard::slotUpPressed()
{
    QList<QTreeWidgetItem*> selectedlayouts = m_layoutstree->selectedItems();
    if (selectedlayouts.size() == 0) {
        return;
    }
    const int selectedrow = selectedlayouts.at(0)->data(0, Qt::UserRole + 1).toInt();
    QList<KKeyboardType> layouts = kGetLayoutsFromTree(m_layoutstree, m_layoutmodel);
    layouts.move(selectedrow, selectedrow - 1);
    kFillTreeFromLayouts(m_layoutstree, layouts);
    emit changed(true);
}

void KCMKeyboard::slotDownPressed()
{
    QList<QTreeWidgetItem*> selectedlayouts = m_layoutstree->selectedItems();
    if (selectedlayouts.size() == 0) {
        return;
    }
    const int selectedrow = selectedlayouts.at(0)->data(0, Qt::UserRole + 1).toInt();
    QList<KKeyboardType> layouts = kGetLayoutsFromTree(m_layoutstree, m_layoutmodel);
    layouts.move(selectedrow, selectedrow + 1);
    kFillTreeFromLayouts(m_layoutstree, layouts);
    emit changed(true);
}

#include "moc_keyboardconfig.cpp"
#include "keyboardconfig.moc"
