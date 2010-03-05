/***************************************************************************
bibleVerse - Daily Bible Verse
Copyright (C) 2009 Paul Walger
This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program; if not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
#include "plasma-bibleverse.h"
#include <QtGui/QPainter>
#include <QtCore/QString>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QLabel>
#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtGui/QAction>
#include <KIcon>
#include <KDE/Plasma/Label>
#include <KDE/Plasma/Svg>
#include <KDE/Plasma/Applet>
#include <KDE/KConfigDialog>
#include <KDE/KSharedConfig>
#include <KDE/Plasma/Theme>
#include <QtDebug>
#ifdef USE_SWOD
#include <swmgr.h>
#include <swmodule.h>
#include <markupfiltmgr.h>
using namespace::sword;
#endif


PlasmaBibleVerse::PlasmaBibleVerse(QObject *parent, const QVariantList &args)
        : Plasma::Applet(parent, args) , m_layout(0L)
{
    setBackgroundHints(StandardBackground);
    setHasConfigurationInterface(true);
    resize(250, 200);
    m_layout = new QGraphicsLinearLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    out = "";
    vdownloader = new verseDownloader(this);
}

PlasmaBibleVerse::~PlasmaBibleVerse()
{
    delete vdownloader;
}

void PlasmaBibleVerse::init()
{
    KConfigGroup cg = config();
    myConfig.translationSource = cg.readEntry("translationSource", 1);
    myConfig.verseSource = cg.readEntry("verseSource", 1);
    myConfig.translationCode = cg.readEntry("translationCode", "10");
    myConfig.showPosition = cg.readEntry("showPosition", true);
    myConfig.fontColor = cg.readEntry("fontColor", "default");
    myConfig.fontSize = cg.readEntry("fontSize", "default");
    myConfig.autoUpdate = cg.readEntry("autoUpdate", 60);

    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    m_label = new Plasma::Label(this);
    m_layout->addItem(m_label);
    loading = false;
    createMenu();
    loadVerse();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadVerse()));
    if(myConfig.autoUpdate != 0)
	timer->start( myConfig.autoUpdate * 1000 * 60);
}
void PlasmaBibleVerse::createConfigurationInterface(KConfigDialog *parent)
{
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));

    QWidget *generalWidget = new QWidget(parent);
    generalConfigUi.setupUi(generalWidget);
    parent->addPage(generalWidget, i18n("General"), "preferences-desktop-locale");

    connect(generalConfigUi.comboBox_translationSource, SIGNAL(currentIndexChanged(int)), this, SLOT(translationConfig(int)));

    QStringList verseSources;
    verseSources << "christnotes.org" << "biblegateway.com";
    generalConfigUi.comboBox_verseSource->clear();
    generalConfigUi.comboBox_verseSource->insertItems(0, verseSources);
    generalConfigUi.comboBox_verseSource->setCurrentIndex(myConfig.verseSource);

    QStringList translationSources;
#ifdef USE_SWORD
    translationSources << "(none)" << "biblegateway.com" << "SWORD Modules";
#else
    translationSources << "(none)" << "biblegateway.com";
#endif
    generalConfigUi.comboBox_translationSource->clear();
    generalConfigUi.comboBox_translationSource->insertItems(0, translationSources);
    generalConfigUi.comboBox_translationSource->setCurrentIndex(myConfig.translationSource);

    translationConfig(myConfig.translationSource);

    QWidget *viewWidget = new QWidget(parent);
    viewConfigUi.setupUi(viewWidget);
    parent->addPage(viewWidget, i18n("View"), "preferences-desktop-display");
    viewConfigUi.checkBox_position->setChecked(myConfig.showPosition);
    connect(viewConfigUi.checkBox_fontColor, SIGNAL(toggled(bool)), viewConfigUi.kcolorbutton_fontColor, SLOT(setDisabled(bool)));
    connect(viewConfigUi.checkBox_fontSize, SIGNAL(toggled(bool)), viewConfigUi.kintspinbox_fontSize, SLOT(setDisabled(bool)));
    if (myConfig.fontColor != "default") {
        viewConfigUi.checkBox_fontColor->setChecked(false);
	viewConfigUi.kcolorbutton_fontColor->setEnabled(true);
	QColor color;
	color.setNamedColor(myConfig.fontColor);
        viewConfigUi.kcolorbutton_fontColor->setColor(color);
    } else {
        viewConfigUi.checkBox_fontColor->setChecked(true);
	viewConfigUi.kcolorbutton_fontColor->setEnabled(false);
    }
    if (myConfig.fontSize != "default") {
        viewConfigUi.checkBox_fontSize->setChecked(false);
	viewConfigUi.kintspinbox_fontSize->setEnabled(true);
        viewConfigUi.kintspinbox_fontSize->setValue(myConfig.fontSize.toInt());
    } else {
        viewConfigUi.checkBox_fontSize->setChecked(true);
	viewConfigUi.kintspinbox_fontSize->setEnabled(false);
    }
    viewConfigUi.kintspinbox_autoUpdate->setValue(myConfig.autoUpdate);
}
void PlasmaBibleVerse::translationConfig(int index)
{
    QStringList translationText;
    if (index == 1) {
        translationText
        << "Amuzgo de Guerrero"
        << "Arabic Life Application Bible"
        << "Bulgarian Bible"
        << "1940 Bulgarian Bible"
        << "Chinanteco de Comaltepec"
        << "Cakchiquel Occidental"
        << "Haitian Creole Version"
        << "Slovo na cestu"
        << "Dette er Biblen pÃ¥ dansk"
        << "Hoffnung für Alle"
        << "Luther Bibel 1545"
        << "New International Version"
        << "New American Standard Bible"
        << "The Message"
        << "Amplified Bible"
        << "New Living Translation"
        << "King James Version"
        << "English Standard Version"
        << "Contemporary English Version"
        << "New King James Version"
        << "New Century Version"
        << "21st Century King James Version"
        << "American Standard Version"
        << "Young's Literal Translation"
        << "Darby Translation"
        << "Holman Christian Standard Bible"
        << "New International Reader's Version"
        << "Wycliffe New Testament"
        << "Worldwide English (New Testament)"
        << "New International Version - UK"
        << "Today's New International Version"
        << "Reina-Valera 1960"
        << "Nueva Versión Internacional"
        << "Reina-Valera 1995"
        << "Castilian"
        << "Reina-Valera Antigua"
        << "Biblia en Lenguaje Sencillo"
        << "La Biblia de las Américas"
        << "Louis Segond"
        << "La Bible du Semeur"
        << "1881 Westcott-Hort New Testament"
        << "1550 Stephanus New Testament"
        << "1894 Scrivener New Testament"
        << "The Westminster Leningrad Codex"
        << "Hiligaynon Bible"
        << "Croatian Bible"
        << "Hungarian KÃ¡roli"
        << "Icelandic Bible"
        << "La Nuova Diodati"
        << "La Parola è Vita"
        << "Jacalteco, Oriental"
        << "Kekchi"
        << "Korean Bible"
        << "Maori Bible"
        << "Macedonian New Testament"
        << "Mam, Central"
        << "Mam de Todos Santos Chuchumatán"
        << "Reimer 2001"
        << "Náhuatl de Guerrero"
        << "Het Boek"
        << "Det Norsk Bibelselskap 1930"
        << "Levande Bibeln"
        << "O Livro"
        << "João Ferreira de Almeida Atualizada"
        << "Quiché, Centro Occidental"
        << "Romanian"
        << "Romanian"
        << "Russian Synodal Version"
        << "Slovo Zhizny"
        << "Nádej pre kazdého"
        << "Albanian Bible"
        << "Levande Bibeln"
        << "Svenska 1917"
        << "Swahili New Testament"
        << "Ang Salita ng Diyos"
        << "Ukrainian Bible"
        << "Uspanteco"
        << "1934 Vietnamese Bible"
        << "Chinese Union Version (Simplified)"
        << "Chinese Union Version (Traditional)";
        translationCode.clear();
        translationCode
        << "94"
        << "28"
        << "21"
        << "82"
        << "90"
        << "98"
        << "23"
        << "29"
        << "11"
        << "33"
        << "10"
        << "31"
        << "49"
        << "65"
        << "45"
        << "51"
        << "9"
        << "47"
        << "46"
        << "50"
        << "78"
        << "48"
        << "8"
        << "15"
        << "16"
        << "77"
        << "76"
        << "53"
        << "73"
        << "64"
        << "72"
        << "60"
        << "42"
        << "61"
        << "41"
        << "6"
        << "57"
        << "59"
        << "2"
        << "32"
        << "68"
        << "69"
        << "70"
        << "81"
        << "71"
        << "62"
        << "17"
        << "18"
        << "55"
        << "34"
        << "103"
        << "104"
        << "20"
        << "24"
        << "122"
        << "88"
        << "107"
        << "56"
        << "109"
        << "30"
        << "5"
        << "35"
        << "37"
        << "25"
        << "111"
        << "14"
        << "38"
        << "13"
        << "39"
        << "40"
        << "1"
        << "44"
        << "7"
        << "75"
        << "43"
        << "27"
        << "113"
        << "19"
        << "80"
        << "22";


    } else if (index == 2) { //SWORD
#ifdef USE_SWORD
        SWMgr library(new MarkupFilterMgr(FMT_PLAIN));

        ModMap::iterator it;
        translationCode.clear();
        translationText.clear();
        for (it = library.Modules.begin(); it != library.Modules.end(); it++) {
            translationText <<  QString((*it).second->Description());
            translationCode <<  QString((*it).second->Name());
        }
#endif
    }
    generalConfigUi.comboBox_translation->clear();
    generalConfigUi.comboBox_translation->insertItems(0, translationText);
    for (int i = 0; i < translationCode.size(); i++) {
        if (myConfig.translationCode == translationCode.at(i)) {
            generalConfigUi.comboBox_translation->setCurrentIndex(i);
        }
    }
}
void PlasmaBibleVerse::configAccepted()
{
    bool changed = false;
    KConfigGroup cg = config();
    if (myConfig.showPosition != viewConfigUi.checkBox_position->isChecked()) {
        myConfig.showPosition = viewConfigUi.checkBox_position->isChecked();
        cg.writeEntry("showPosition", myConfig.showPosition);
        changed = true;
    }

    if (myConfig.verseSource != generalConfigUi.comboBox_verseSource->currentIndex()) {
        myConfig.verseSource = generalConfigUi.comboBox_verseSource->currentIndex();
        cg.writeEntry("verseSource", myConfig.verseSource);
        changed = true;
    }

    if (myConfig.translationSource != generalConfigUi.comboBox_translationSource->currentIndex()) {
        myConfig.translationSource = generalConfigUi.comboBox_translationSource->currentIndex();
        cg.writeEntry("translationSource", myConfig.translationSource);
        changed = true;
    }
    if (generalConfigUi.comboBox_translation->currentIndex() != -1) {
        qDebug() << " PlasmaBibleVerse::configAccepted() translation = " << generalConfigUi.comboBox_translation->currentIndex();
        if (myConfig.translationCode != translationCode.at(generalConfigUi.comboBox_translation->currentIndex())) {
            myConfig.translationCode = translationCode.at(generalConfigUi.comboBox_translation->currentIndex());
            cg.writeEntry("translationCode", myConfig.translationCode);
            changed = true;
        }
    }
    if (!viewConfigUi.checkBox_fontSize->isChecked() && viewConfigUi.kintspinbox_fontSize->value() != 0) {
        if (myConfig.fontSize != QString::number(viewConfigUi.kintspinbox_fontSize->value())) {
            myConfig.fontSize = QString::number(viewConfigUi.kintspinbox_fontSize->value());
            cg.writeEntry("fontSize", myConfig.fontSize);
            changed = true;
        }
    } else {
        if (myConfig.fontSize != "default") {
            myConfig.fontSize = "default";
            cg.writeEntry("fontSize", "default");
            changed = true;
        }
    }

    if (!viewConfigUi.checkBox_fontColor->isChecked() && viewConfigUi.kcolorbutton_fontColor->color().name() != "") {
        if (myConfig.fontColor != viewConfigUi.kcolorbutton_fontColor->color().name()) {
            myConfig.fontColor = viewConfigUi.kcolorbutton_fontColor->color().name();
            cg.writeEntry("fontColor", myConfig.fontColor);
            changed = true;
        }
    } else {
        if (myConfig.fontColor != "default") {
            myConfig.fontColor = "default";
            cg.writeEntry("fontColor", "default");
            changed = true;
        }
    }
	if (myConfig.autoUpdate != viewConfigUi.kintspinbox_autoUpdate->value()) {
            myConfig.autoUpdate = viewConfigUi.kintspinbox_autoUpdate->value();
	     
            cg.writeEntry("autoUpdate", myConfig.autoUpdate);
	    timer->stop();
	    if(myConfig.autoUpdate != 0)
		timer->start( myConfig.autoUpdate * 1000 * 60);
	   
        }
    if (changed == true) {
        loadVerse();
    }
    emit configNeedsSaving();

}
void PlasmaBibleVerse::showVerse(QString text, QString pos)
{
    /*QDate localDate = QDate::currentDate();
    int year = localDate.year();
    int day = localDate.dayOfYear();
    lastDate = QString::number(year) + "-" + QString::number(day);*/

    setBusy(false);
    //qDebug() << "PlasmaBibleVerse::showVerse() out:"<<out;
    if (myConfig.fontColor != "default" || myConfig.fontSize != "default") {
        qDebug() << "fontColor or fontsize not default";
        QString add = " style = \"";
        if (myConfig.fontColor != "default") {
            qDebug() << " font color = " << myConfig.fontColor;
            add += " color:" + myConfig.fontColor + ";";
        }
        if (myConfig.fontSize != "default") {
            add += " font-size:" + myConfig.fontSize + "px; ";
        }
        add += "\"";
        text = "<span" + add + ">" + text + "</span>";
    }
    if (myConfig.showPosition) {
        text = text + "\n<br>" + "<font size =\"1\"><i>" + pos + "</i></font>";
    }
    qDebug() << text;
    out = text;

    update();
    loading = false;
}
void PlasmaBibleVerse::loadVerse()
{
    loading = true;
    setBusy(true);
    vdownloader->setConfig(myConfig);
    vdownloader->downloadNew();
    QObject::connect(vdownloader, SIGNAL(newVerse(QString, QString)), this, SLOT(showVerse(QString, QString)));
}
void PlasmaBibleVerse::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    /*QDate localDate = QDate::currentDate();
    int year = localDate.year();
    int day = localDate.dayOfYear();
    QString currentDate = QString::number(year) + "-" + QString::number(day);
    if(lastDate != currentDate && loading == false)
        loadVerse();*/
    Q_UNUSED(option);
    Q_UNUSED(contentsRect);
    if (out != "") {
        p->setRenderHint(QPainter::SmoothPixmapTransform);
        p->setRenderHint(QPainter::Antialiasing);
        setBusy(false);
        QLabel * tLabel;
        tLabel = m_label->nativeWidget();
        tLabel->setTextFormat(Qt::RichText);
        m_label->setText(out);
    };
}
void PlasmaBibleVerse::createMenu()
{
    actions.clear();
    QAction *reload = new QAction(i18n("Reload"), this);
    reload->setIcon(KIconLoader::global()->loadIcon("view-refresh", KIconLoader::NoGroup,
                    48, KIconLoader::DefaultState, QStringList(), 0L, false));
    actions.append(reload);
    connect(reload, SIGNAL(triggered(bool)), this, SLOT(loadVerse()));
}
QList<QAction*> PlasmaBibleVerse::contextualActions()
{
    return actions;
}
#include "plasma-bibleverse.moc"
