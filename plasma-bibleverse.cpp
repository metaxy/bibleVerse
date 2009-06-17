/***************************************************************************
 *   Copyright (C) 2008 by Paul Walger   *
 *   paul@walger.name   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "plasma-bibleverse.h"
#include "versedownloader.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QString>
#include <QGraphicsLinearLayout>
#include <QLabel>
#include <QGraphicsScene>
#include <QTextEdit>
#include <QDate>
#include <QList>
#include <QAction>
#include <Plasma/Label>
#include <Plasma/Svg>
#include <Plasma/Applet>
#include <KConfigDialog>
#include <KSharedConfig>
#include <plasma/theme.h>
 
PlasmaBibleVerse::PlasmaBibleVerse(QObject *parent, const QVariantList &args)
: Plasma::Applet(parent, args) ,m_layout(0L)
{
    setBackgroundHints(DefaultBackground);
	setHasConfigurationInterface( true );
	resize(250, 200);
	m_layout = new QGraphicsLinearLayout(this);
	m_layout->setContentsMargins(0,0,0,0);
	m_layout->setSpacing(0);
	myConfig.translationSource = 1;
	myConfig.verseSource = 1;
	myConfig.showPosition = true;
	myConfig.translationCode = "10";
	
	KConfigGroup cg = config();
	myConfig.translationSource = cg.readEntry("translationSource", 0);
	myConfig.verseSource = cg.readEntry("verseSource", 1);
	myConfig.translationCode = cg.readEntry("translationCode", "");
	myConfig.showPosition = cg.readEntry("showPosition", true);
	
	out = "";
	loadVerse();
}
 
PlasmaBibleVerse::~PlasmaBibleVerse()
{
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
    }
}
 
void PlasmaBibleVerse::init()
{
	setAspectRatioMode( Plasma::IgnoreAspectRatio );
	m_label = new Plasma::Label(this);
	m_layout->addItem(m_label);
	loading = false;
	createMenu();
} 
void PlasmaBibleVerse::createConfigurationInterface(KConfigDialog *parent)
{
	QWidget *widget = new QWidget(parent);
	configUi.setupUi(widget);
	parent->addPage(widget, i18n("General"), Applet::icon());
	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
	configUi.checkBox_position->setChecked(true);
	QStringList verseSources;
	verseSources << "christnotes.org" << "biblegateway.com";
	configUi.comboBox_verseSource->clear();
	configUi.comboBox_verseSource->insertItems(0,verseSources);
	configUi.comboBox_verseSource->setCurrentIndex(myConfig.verseSource);
	
	QStringList translationSources;
	translationSources << "(none)" << "biblegateway.com";
	configUi.comboBox_translationSource->clear();
	configUi.comboBox_translationSource->insertItems(0,translationSources);
	configUi.comboBox_translationSource->setCurrentIndex(myConfig.translationSource);
	
	configUi.lineEdit_translationCode->setText(myConfig.translationCode);
	
	if(myConfig.showPosition == false)
	{
		configUi.checkBox_position->setCheckState(Qt::Unchecked);
	}
	else
	{
		configUi.checkBox_position->setCheckState(Qt::Checked);
	}

}
void PlasmaBibleVerse::configAccepted()
{
	bool changed = false;
	KConfigGroup cg = config();
	qDebug() << configUi.checkBox_position->isChecked();
	if (myConfig.showPosition != configUi.checkBox_position->isChecked()) 
	{
		myConfig.showPosition = configUi.checkBox_position->isChecked();
		cg.writeEntry("showPosition", myConfig.showPosition);
		changed = true;
	}
	
	if (myConfig.verseSource != configUi.comboBox_verseSource->currentIndex()) 
	{
		myConfig.verseSource = configUi.comboBox_verseSource->currentIndex();
		cg.writeEntry("verseSource", myConfig.verseSource);
		changed = true;
	}
	
	if (myConfig.translationSource != configUi.comboBox_translationSource->currentIndex()) 
	{
		myConfig.translationSource = configUi.comboBox_translationSource->currentIndex();
		cg.writeEntry("translationSource", myConfig.translationSource);
		changed = true;
	}
	
	if (myConfig.translationCode != configUi.lineEdit_translationCode->text()) 
	{
		myConfig.translationCode = configUi.lineEdit_translationCode->text();
		cg.writeEntry("translationCode", myConfig.translationCode);
		changed = true;
	}
	
	loadVerse();
	emit configNeedsSaving();
	
}
void PlasmaBibleVerse::showVerse(QString text,QString pos)
{
	/*QDate localDate = QDate::currentDate();
	int year = localDate.year();
	int day = localDate.dayOfYear();
	lastDate = QString::number(year) + "-" + QString::number(day);*/

	setBusy( false );
	qDebug() << "PlasmaBibleVerse::showVerse() out:"<<out;
	if(myConfig.showPosition)
	{
		out = text+"\n<br>"+"<font size=\"-1\"><i>"+pos+"</i></font>";
	}
	else
	{
		out = text;
	}
	update();
	loading = false;
}
void PlasmaBibleVerse::loadVerse()
{
	loading = true;
	setBusy( true );
	verseDownloader *vdownloader;
	vdownloader = new verseDownloader( this );
	vdownloader->setConfig(&myConfig);
	vdownloader->downloadNew();
	QObject::connect( vdownloader, SIGNAL( newVerse( QString, QString ) ), this, SLOT( showVerse( QString, QString ) ) );
	
}
void PlasmaBibleVerse::paintInterface(QPainter *p,const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	/*QDate localDate = QDate::currentDate();
	int year = localDate.year();
	int day = localDate.dayOfYear();
	QString currentDate = QString::number(year) + "-" + QString::number(day);
	if(lastDate != currentDate && loading == false)
		loadVerse();*/
	myPaint(p,option,contentsRect);
}
void PlasmaBibleVerse::createMenu()
{
	actions.clear();

	QAction *reload = new QAction(tr("Reload Verse"), this );
	reload->setIcon( KIconLoader::global()->loadIcon( "view-refresh", KIconLoader::NoGroup,
		48, KIconLoader::DefaultState, QStringList(), 0L, false ) );
	actions.append( reload );
	connect( reload, SIGNAL( triggered(bool) ), this, SLOT( loadVerse() ) );
}
QList<QAction*> PlasmaBibleVerse::contextualActions()
{
	return actions;
}
void PlasmaBibleVerse::myPaint(QPainter *p,const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	Q_UNUSED(option);
	Q_UNUSED(contentsRect);
	if(out != "")
	{
		p->setRenderHint(QPainter::SmoothPixmapTransform);
		p->setRenderHint(QPainter::Antialiasing);
		setBusy( false );
		QLabel * tLabel;
		tLabel = m_label->nativeWidget();
		tLabel->setTextFormat(Qt::RichText);
		m_label->setText(out);
	}
}
#include "plasma-bibleverse.moc" 
