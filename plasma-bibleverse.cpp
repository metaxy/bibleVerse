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
	setBackgroundHints(StandardBackground);
	setHasConfigurationInterface( true );
	resize(250,200);
	m_layout = new QGraphicsLinearLayout(this);
	m_layout->setContentsMargins(0,0,0,0);
	m_layout->setSpacing(0);
	out = "";
}
 
PlasmaBibleVerse::~PlasmaBibleVerse()
{
 //nothing
}
 
void PlasmaBibleVerse::init()
{
	KConfigGroup cg = config();
	myConfig.translationSource = cg.readEntry("translationSource", 1);
	myConfig.verseSource = cg.readEntry("verseSource", 1);
	myConfig.translationCode = cg.readEntry("translationCode", "10");
	myConfig.showPosition = cg.readEntry("showPosition", true);
	
	setAspectRatioMode( Plasma::IgnoreAspectRatio );
	m_label = new Plasma::Label(this);
	m_layout->addItem(m_label);
	loading = false;
	createMenu();
	loadVerse();
} 
void PlasmaBibleVerse::createConfigurationInterface(KConfigDialog *parent)
{
	QWidget *widget = new QWidget(parent);
	configUi.setupUi(widget);
	parent->addPage(widget, i18n("General"), Applet::icon());
	connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
	connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
	connect(configUi.comboBox_translationSource, SIGNAL(currentIndexChanged(int)), this, SLOT(translationConfig(int)));
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
	
	translationConfig(myConfig.translationSource);
	if(myConfig.showPosition == false)
	{
		configUi.checkBox_position->setCheckState(Qt::Unchecked);
	}
	else
	{
		configUi.checkBox_position->setCheckState(Qt::Checked);
	}

}
void PlasmaBibleVerse::translationConfig(int index)
{
	//qDebug() << "translationConfig() index = " << index;
	QStringList translationText;
	if(index == 1)
	{
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
			<<"94"
			<<"28"
			<<"21"
			<<"82"
			<<"90"
			<<"98"
			<<"23"
			<<"29"
			<<"11" 
			<<"33" 
			<<"10"
			<<"31"        
			<<"49"      
			<<"65" 
			<<"45" 
			<<"51" 
			<<"9" 
			<<"47"  
			<<"46"  
			<<"50" 
			<<"78" 
			<<"48"    
			<<"8"    
			<<"15"       
			<<"16"   
			<<"77"     
			<<"76" 
			<<"53"   
			<<"73" 
			<<"64"   
			<<"72" 
			<<"60"     
			<<"42"    
			<<"61"   
			<<"41"     
			<<"6"
			<<"57"  
			<<"59"    
			<<"2" 
			<<"32"   
			<<"68"   
			<<"69"  
			<<"70"  
			<<"81"   
			<<"71"   
			<<"62"   
			<<"17"   
			<<"18" 
			<<"55"      
			<<"34"        
			<<"103"      
			<<"104"   
			<<"20"     
			<<"24"         
			<<"122"         
			<<"88" 
			<<"107" 
			<<"56" 
			<<"109"     
			<<"30"       
			<<"5"       
			<<"35"      
			<<"37"        
			<<"25"       
			<<"111"     
			<<"14"   
			<<"38"    
			<<"13"       
			<<"39"    
			<<"40"     
			<<"1"     
			<<"44" 
			<<"7"    
			<<"75"  
			<<"43"  
			<<"27"  
			<<"113"  
			<<"19"   
			<<"80"
			<<"22";


	}
	configUi.comboBox_translation->clear();
	configUi.comboBox_translation->insertItems(0,translationText);
	for(int i=0;i<translationCode.size();i++)
	{
		if(myConfig.translationCode == translationCode.at(i))
		{
			configUi.comboBox_translation->setCurrentIndex(i);
		}
	}		
}
void PlasmaBibleVerse::configAccepted()
{
	bool changed = false;
	KConfigGroup cg = config();
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
	
	if (myConfig.translationCode != translationCode.at(configUi.comboBox_translation->currentIndex())) 
	{
		myConfig.translationCode = translationCode.at(configUi.comboBox_translation->currentIndex());
		//qDebug() << "PlasmaBibleVerse::configAccepted() translationCode = " << myConfig.translationCode;
		cg.writeEntry("translationCode", myConfig.translationCode);
		changed = true;
	}
	if(changed == true)
	{
		loadVerse();
	}
	emit configNeedsSaving();
	
}
void PlasmaBibleVerse::showVerse(QString text,QString pos)
{
	/*QDate localDate = QDate::currentDate();
	int year = localDate.year();
	int day = localDate.dayOfYear();
	lastDate = QString::number(year) + "-" + QString::number(day);*/

	setBusy( false );
	//qDebug() << "PlasmaBibleVerse::showVerse() out:"<<out;
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
	};
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
#include "plasma-bibleverse.moc" 
