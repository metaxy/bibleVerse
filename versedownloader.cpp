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


#include "versedownloader.h"
#include "web.h"
#include <QtDebug>
#include <QTextDocument>
#include <QMap>
#include <QString>

verseDownloader::verseDownloader( QObject *parent ) : QObject( parent )
{

}
void verseDownloader::downloadNew( void )
{
	qDebug() << "verseDownloader::downloadNew() from " << config.verseSource;
	//load site
	switch (config.verseSource)
	{
		case 0:
			w = new web(this );
			QObject::connect( w, SIGNAL( sdone( QString, QString ) ), this, SLOT( pharseSourceSite( QString, QString ) ) );
			w->get(QString("http://www.christnotes.org/dbv.php"));
			break;
		case 1:
			w = new web( this );
			QObject::connect( w, SIGNAL( sdone( QString, QString ) ), this, SLOT( pharseSourceSite( QString, QString ) ) );
			w->get(QString("http://www.biblegateway.com/votd/get/?format=html"));
			break;
	}
}
void verseDownloader::pharseSourceSite(QString out,QString header)
{
	Q_UNUSED(header); 
	qDebug() << "verseDownloader::pharseSourceSite() from " << config.verseSource;
	//pharse out
	QString text="",pos="",searchstring;
	QString bout = out;
	QString bout2 = out;
	int pos1,pos2;
	switch (config.verseSource)
	{
		case 0://christnotes.org
			searchstring = "dbv-content\">",
			pos1 = bout.indexOf(searchstring,0);
			pos2 = bout.indexOf("</div>",pos1);
			text = bout.remove(pos2,out.size());
			text = text.remove(0,pos1+searchstring.size());
			
			searchstring = "dbv-reference\">",
			pos1 = bout2.indexOf(searchstring,0);
			pos2 = bout2.indexOf("<span class",pos1);
			pos = bout2.remove(pos2,out.size());
			pos = pos.remove(0,pos1+searchstring.size());
			qDebug() << "verseDownloader::pharseSourceSite() source pos:" << pos;
			if(config.translationSource != 0)
			{
				translate(text,pos);
			}
			else
			{
				emit newVerse(text,pos);
			}
			break;
		case 1:
			QString a = bout.remove(bout.indexOf("</a>)",0),bout.size());
			a = a.remove("<div>");
			pos = a;
			text = a.remove(a.indexOf("(<a",0),a.size());
			pos = pos.remove(0,pos.indexOf("\">",0)+2);
			qDebug() << "verseDownloader::pharseSourceSite() source pos:" << pos;
			if(config.translationSource != 0)
			{
				translate(text,pos);
			}
			else
			{
				emit newVerse(text,pos+"( from <a href=\"http://www.biblegateway.com\">biblegateway.com</a> )");
			}
			break;
	}
	//emit newVerse(text,pos);
}
void verseDownloader::translate( QString text,QString pos )
{
	Q_UNUSED(text);
	QString url;
	struct pos p;
	QString newPos;
	switch (config.translationSource)
	{
		case 1://biblegateway.com
			w = new web( this );
			QObject::connect( w, SIGNAL( sdone( QString, QString ) ), this, SLOT( pharseTranslationsSite( QString, QString ) ) );
			if(config.verseSource == 1)
			{
				//qDebug() << "verseDownloader::translate()1 code = " << config.translationCode;
				url = "http://www.biblegateway.com/votd/get/?format=html&version="+config.translationCode;
			}
			else
			{
				
				p = convertPosition2Uni(pos,config.verseSource);
				newPos = convertUni2Position(p,config.translationSource);
				//qDebug() << "verseDownloader::translate()2 code = " << config.translationCode;
				url = "http://www.biblegateway.com/passage/?search="+newPos+";&version="+config.translationCode+";&interface=print";
			}
			
			w->get(url);
			break;
	}

}
void verseDownloader::pharseTranslationsSite(QString out,QString header)
{
	Q_UNUSED(header);
	//qDebug() << "verseDownloader::pharseTranslationsSite()";
	if(out.contains("Error:"))
	{
		qDebug() << "verseDownloader::pharseTranslationsSite() Error";
		emit newVerse("Error while loading verse. Please try an another translation.","");
		return;
	}
	//pharse out
	QString text = " ",pos = " ",searchstring;
	QString bout = out;
	QString bout2 = out;
	int pos1,pos2;
	QTextDocument doc;
	switch (config.translationSource)
	{
		case 1://biblegateway.com
			if(config.verseSource == 1)
			{
				QString a = bout.remove(bout.indexOf("</a>)",0),bout.size());
				a = a.remove("<div>");
				pos = a;
				text = a.remove(a.indexOf("(<a",0),a.size());
				pos = pos.remove(0,pos.indexOf("\">",0)+2);
				emit newVerse(text,pos+"( from <a href=\"http://www.biblegateway.com\">biblegateway.com</a> )");
			}
			else
			{
			
				searchstring = "<div class=\"result-text-style-normal\">",
				pos1 = bout.indexOf(searchstring,0);
				pos2 = bout.indexOf("</p>",pos1);
				text = bout.remove(pos2,out.size());
				text = text.remove(0,pos1+searchstring.size());
				if(pos1 == -1 || pos2 == -1)
				{
					qDebug() << "verseDownloader::pharseTranslationsSite() pharse Error" ;
					return;
				}
				//qDebug() << "pos1:" << pos1<< "pos2"<<pos2;
				searchstring = "</sup>";
				pos1 = text.indexOf(searchstring,0);
				text = text.remove(0,pos1+searchstring.size());
				doc.setHtml(text);
				text = doc.toPlainText();//remove all Html-Tags
				
				searchstring = "</div>\n<h3>";
				pos1 = bout2.indexOf(searchstring,0);
				pos2 = bout2.indexOf("</h3>",pos1);
				
				if(pos1 == -1 || pos2 == -1)
				{
					qDebug() << "verseDownloader::pharseTranslationsSite() pharse Error" ;
					return;
				}
				pos = bout2.remove(pos2,out.size());
				pos = pos.remove(0,pos1+searchstring.size());
				doc.setHtml(pos);
				
				pos = doc.toPlainText();
				//qDebug() << "verseDownloader::pharseTranslationsSite() verse:"<< text;
				emit newVerse(text,pos+"( from <a href=\"http://www.biblegateway.com\">biblegateway.com</a> )");
			}
			break;
		default:
			break;
	}
	//emit newVerse(text,pos);
}
void verseDownloader::setConfig( struct configStruct *newConfig)
{
	config = *newConfig;
}
verseDownloader::~verseDownloader()
{
	
}
struct pos verseDownloader::convertPosition2Uni(QString pos,int from)
{
	//qDebug() << "verseDownloader::convertPosition2Uni() from:"<< from <<" pos:" << pos;
	struct pos uPos;
	switch (from)
	{
		case 0://christnotes.org
		case 1://biblegateway.com
			QString bpos = pos;
			QString bpos2 = pos;
			int point = bpos.lastIndexOf(" ");
			QString bookName =  bpos2.remove(point,pos.size());
			QString rest =  bpos.remove(0,point); 
			if(point == -1)
			{
				//qDebug() << "verseDownloader::convertPosition2Uni() pharse Error while step 1";
				return uPos;
			}
			
			QStringList list_2 = rest.split(":");
			if(list_2.size() < 2)
			{
				//qDebug() << "verseDownloader::convertPosition2Uni() pharse Error while step 2, rest:"<<rest << " bookName"<<bookName ;
				return uPos;
			}
			QString chapter = list_2.at(0);
			QString verse = list_2.at(1);
			chapter.remove(" ");
			verse.remove(" ");
			//qDebug() << "verseDownloader::convertPosition2Uni() chapter:"<<chapter << " verse:"<<verse;
			
			QMap<QString, int> bookMap;
			bookMap["Genesis"] = 1;
			bookMap["Exodus"] = 2;
			bookMap["Leviticus"] = 3;
			bookMap["Numbers"] = 4;
			bookMap["Deuteronomy"] = 5; 
			bookMap["Joshua"] = 6; 
			bookMap["Judges"] = 7; 
			bookMap["Ruth"] = 8; 
			bookMap["1 Samuel"] = 9; 
			bookMap["2 Samuel"] = 10; 
			bookMap["1 Kings"] = 11; 
			bookMap["2 Kings"] = 12; 
			bookMap["1 Chronicles"] = 13; 
			bookMap["2 Chronicles"] = 14; 
			bookMap["Ezrav"] = 15; 
			bookMap["Nehemiah"] = 16; 
			bookMap["Esther"] = 17; 
			bookMap["Job"] = 18; 
			bookMap["Psalm"] = 19; 
			bookMap["Proverbs"] = 20 ; 
			bookMap["Ecclesiastes"] = 21 ; 
			bookMap["Song of Solomon"] = 22;
			bookMap["Isaiah"] = 23; 
			bookMap["Jeremiah"] = 24; 
			bookMap["Lamentations"] = 25; 
			bookMap["Ezekiel"] = 26; 
			bookMap["Daniel"] = 27; 
			bookMap["Hosea"] = 28; 
			bookMap["Joel"] = 29; 
			bookMap["Amos"] = 30; 
			bookMap["Obadiah"] = 31; 
			bookMap["Jonah"] = 32; 
			bookMap["Micah"] = 33; 
			bookMap["Nahum"] = 34; 
			bookMap["Habakkuk"] = 35;
			bookMap["Zephaniah"] = 36; 
			bookMap["Haggai"] = 37; 
			bookMap["Zechariah"] = 38; 
			bookMap["Malachi"] = 39; 
			bookMap["Matthew"] = 40;
			bookMap["Mark"] = 41;
			bookMap["Luke"] = 42;
			bookMap["John"] = 43;
			bookMap["Acts"] = 44;
			bookMap["Romans"] = 45;
			bookMap["1 Corinthians"] = 46;
			bookMap["2 Corinthians"] = 47;
			bookMap["Galatians"] = 48;
			bookMap["Ephesians"] = 49;
			bookMap["Philippians"] = 50;
			bookMap["Colossians"] = 51;
			bookMap["1 Thessalonians"] = 52;
			bookMap["2 Thessalonians"] = 53;
			bookMap["1 Timothy"] = 54;
			bookMap["2 Timothy"] = 55;
			bookMap["Titus"] = 56;
			bookMap["Philemon"] = 57;
			bookMap["Hebrews"] = 58;
			bookMap["James"] = 59;
			bookMap["1 Peter"] = 60;
			bookMap["2 Peter"] = 61;
			bookMap["1 John"] = 62;
			bookMap["2 John"] = 63;
			bookMap["3 John"] = 64;
			bookMap["Jude"] = 65;
			bookMap["Revelation"] = 66;
			uPos.bookID = bookMap[bookName];
			uPos.bookName = bookName;
			uPos.chapterID = chapter.toInt();
			//qDebug() <<  "verseDownloader::convertPosition2Uni() bookID:" << uPos.bookID << " bookName:" << uPos.bookName << " ";
			if(verse.contains("-")) 
			{
				//qDebug() << "verseDownloader::convertPosition2Uni() more than one verse ( verse:"<< verse << ")";
				QStringList one = verse.split("-");
				uPos.verseID =  one.at(0).toInt();
				uPos.verseStartID = one.at(0).toInt();
				uPos.verseEndID = one.at(1).toInt();
				//qDebug() << "verseDownloader::convertPosition2Uni() verseStartID:"<<uPos.verseStartID << "verseeEndID"<<uPos.verseEndID;
			}
			else
			{
				uPos.verseID = verse.toInt();
				uPos.verseStartID = verse.toInt();
				uPos.verseEndID = verse.toInt();
			}
			return uPos;
			break;
	}
	return uPos;
}
QString verseDownloader::convertUni2Position(struct pos uPos,int to)
{
	qDebug() << "verseDownloader::convertUni2Position to:" << to;
	switch (to)
	{
		case 0://christnotes.org
		case 1://biblegateway.com
			QMap<QString, int> bookMap;
			bookMap["Genesis"] = 1;
			bookMap["Exodus"] = 2;
			bookMap["Leviticus"] = 3;
			bookMap["Numbers"] = 4;
			bookMap["Deuteronomy"] = 5; 
			bookMap["Joshua"] = 6; 
			bookMap["Judges"] = 7; 
			bookMap["Ruth"] = 8; 
			bookMap["1 Samuel"] = 9; 
			bookMap["2 Samuel"] = 10; 
			bookMap["1 Kings"] = 11; 
			bookMap["2 Kings"] = 12; 
			bookMap["1 Chronicles"] = 13; 
			bookMap["2 Chronicles"] = 14; 
			bookMap["Ezrav"] = 15; 
			bookMap["Nehemiah"] = 16; 
			bookMap["Esther"] = 17; 
			bookMap["Job"] = 18; 
			bookMap["Psalm"] = 19; 
			bookMap["Proverbs"] = 20 ; 
			bookMap["Ecclesiastes"] = 21 ; 
			bookMap["Song of Solomon"] = 22;
			bookMap["Isaiah"] = 23; 
			bookMap["Jeremiah"] = 24; 
			bookMap["Lamentations"] = 25; 
			bookMap["Ezekiel"] = 26; 
			bookMap["Daniel"] = 27; 
			bookMap["Hosea"] = 28; 
			bookMap["Joel"] = 29; 
			bookMap["Amos"] = 30; 
			bookMap["Obadiah"] = 31; 
			bookMap["Jonah"] = 32; 
			bookMap["Micah"] = 33; 
			bookMap["Nahum"] = 34; 
			bookMap["Habakkuk"] = 35;
			bookMap["Zephaniah"] = 36; 
			bookMap["Haggai"] = 37; 
			bookMap["Zechariah"] = 38; 
			bookMap["Malachi"] = 39; 
			bookMap["Matthew"] = 40;
			bookMap["Mark"] = 41;
			bookMap["Luke"] = 42;
			bookMap["John"] = 43;
			bookMap["Acts"] = 44;
			bookMap["Romans"] = 45;
			bookMap["1 Corinthians"] = 46;
			bookMap["2 Corinthians"] = 47;
			bookMap["Galatians"] = 48;
			bookMap["Ephesians"] = 49;
			bookMap["Philippians"] = 50;
			bookMap["Colossians"] = 51;
			bookMap["1 Thessalonians"] = 52;
			bookMap["2 Thessalonians"] = 53;
			bookMap["1 Timothy"] = 54;
			bookMap["2 Timothy"] = 55;
			bookMap["Titus"] = 56;
			bookMap["Philemon"] = 57;
			bookMap["Hebrews"] = 58;
			bookMap["James"] = 59;
			bookMap["1 Peter"] = 60;
			bookMap["2 Peter"] = 61;
			bookMap["1 John"] = 62;
			bookMap["2 John"] = 63;
			bookMap["3 John"] = 64;
			bookMap["Jude"] = 65;
			bookMap["Revelation"] = 66;
			QString qReturnString;
			if(uPos.verseStartID != uPos.verseEndID)
			{
				QString bookName;
				QMapIterator<QString, int> i(bookMap);
				while (i.hasNext()) 
				{
					i.next();
					if(i.value() == uPos.bookID)
					{
						bookName = i.key();
						break;
					}
				}
				//qDebug() << "verseDownloader::convertUni2Position()verseStartID:"<<uPos.verseStartID << "verseeEndID"<<uPos.verseEndID;
				qReturnString = bookName + " "+QString::number(uPos.chapterID)+":"+QString::number(uPos.verseStartID)+"-"+QString::number(uPos.verseEndID);
				//qDebug() << "verseDownloader::convertUni2Position() more than one verse qReturnString"<<qReturnString;
				//todo:more than one verse
			}
			else
			{
				QString bookName;
				QMapIterator<QString, int> i(bookMap);
				while (i.hasNext()) 
				{
					i.next();
					if(i.value() == uPos.bookID)
					{
						bookName = i.key();
						break;
					}
				}
				qReturnString = bookName + " "+QString::number(uPos.chapterID)+":"+QString::number(uPos.verseID);
			}
			//qDebug() << "verseDownloader::convertUni2Position() qReturnString:"<<qReturnString;
			return qReturnString;
			break;
	}
	return "";
}