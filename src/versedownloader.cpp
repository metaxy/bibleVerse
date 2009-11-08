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
#include <QtCore/QtDebug>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <swmgr.h>
#include <swmodule.h>
#include <markupfiltmgr.h>
#include <versekey.h>
#include <listkey.h>
#include <klocalizedstring.h>
#include <kdebug.h>

using sword::SWMgr;
using sword::VerseKey;
using sword::ListKey;
using sword::SWModule;
using sword::SW_POSITION;
using sword::FMT_PLAIN;
using sword::MarkupFilterMgr;
using namespace::sword;
struct pos {
    int bookID;
    QString bookName;
    int chapterID;
    int chapterStartID;
    int chapterEndID;
    int verseID;
    int verseStartID;
    int verseEndID;

};
verseDownloader::verseDownloader(QObject *parent) : QObject(parent)
{

}
void verseDownloader::downloadNew(void)
{
    qDebug() << "verseDownloader::downloadNew() from " << config.verseSource;
    //load site
    KIO::TransferJob *job = NULL;
    switch (config.verseSource) {
    case 0:
        downloadedData.clear();
        job = KIO::get(KUrl("http://www.christnotes.org/dbv.php"), KIO::Reload, KIO::HideProgressInfo);
        connect(job, SIGNAL(data(KIO::Job *, const QByteArray &)), this, SLOT(downloaded(KIO::Job *, const QByteArray &)));
        connect(job, SIGNAL(result(KJob *)), this, SLOT(pharseSourceSite()));
        break;
    case 1:
        downloadedData.clear();
        job = KIO::get(KUrl("http://www.christnotes.org/dbv.php"), KIO::Reload, KIO::HideProgressInfo);
        connect(job, SIGNAL(data(KIO::Job *, const QByteArray &)), this, SLOT(downloaded(KIO::Job *, const QByteArray &)));
        connect(job, SIGNAL(result(KJob *)), this, SLOT(pharseSourceSite()));
        break;
    }
}
void verseDownloader::downloaded(KIO::Job *job, const QByteArray &data)
{
    QString out = QString::fromLocal8Bit(data);
    downloadedData += out;
}
void verseDownloader::pharseSourceSite()
{
    QString out = downloadedData;
    qDebug() << "verseDownloader::pharseSourceSite() from " << config.verseSource << " translationSource = " << config.translationSource << " out.size = " << out.size();
    //pharse out
    QString text = "", pos = "", searchstring;
    QString bout = out;
    QString bout2 = out;
    int pos1, pos2;
    switch (config.verseSource) {
    case 0://christnotes.org
        searchstring = "dbv-content\">";
        pos1 = bout.indexOf(searchstring, 0);
        pos2 = bout.indexOf("</div>", pos1);
        text = bout.remove(pos2, out.size());
        text = text.remove(0, pos1 + searchstring.size());

        searchstring = "dbv-reference\">";
        pos1 = bout2.indexOf(searchstring, 0);
        pos2 = bout2.indexOf("<span class", pos1);
        pos = bout2.remove(pos2, out.size());
        pos = pos.remove(0, pos1 + searchstring.size());
        //qDebug() << "verseDownloader::pharseSourceSite() source pos:" << pos;
        if (config.translationSource != 0) {
            translate(text, pos);
        } else {
            emit newVerse(text, pos);
        }
        break;
    case 1:
        QString a = bout.remove(bout.indexOf("</a>)", 0), bout.size());
        a = a.remove("<div>");
        pos = a;
        text = a.remove(a.indexOf("(<a", 0), a.size());
        pos = pos.remove(0, pos.indexOf("\">", 0) + 2);
        if (config.translationSource != 0) {
            translate(text, pos);
        } else {
            emit newVerse(text, pos + "( from <a href=\"http://www.biblegateway.com\">biblegateway.com</a> )");
        }
        break;
    }
    //emit newVerse(text,pos);
}
void verseDownloader::translate(QString text, QString pos)
{
    Q_UNUSED(text);
    QString url;
    struct pos p;
    QString newPos;
    KIO::TransferJob *job = NULL;
    switch (config.translationSource) {
    case 1://biblegateway.com
        if (config.verseSource == 1) {
            url = "http://www.biblegateway.com/votd/get/?format=html&version=" + config.translationCode;
        } else {
            p = convertPosition2Uni(pos, config.verseSource);
            newPos = convertUni2Position(p, config.translationSource);
            url = "http://www.biblegateway.com/passage/?search=" + newPos + ";&version=" + config.translationCode + ";&interface=print";
        }
        downloadedData.clear();
        job = KIO::get(KUrl(url), KIO::Reload, KIO::HideProgressInfo);
        connect(job, SIGNAL(data(KIO::Job *, const QByteArray &)), this, SLOT(downloaded(KIO::Job *, const QByteArray &)));
        connect(job, SIGNAL(result(KJob *)), this, SLOT(pharseTranslationsSite()));
        break;
    case 2://SWORD Module
        SWMgr library(new MarkupFilterMgr(FMT_PLAIN));
        SWModule *target;
        struct pos mPos = convertPosition2Uni(pos, config.verseSource);
        QString myPos = convertUni2Position(mPos, config.translationSource);
        qDebug() << "verseDownloader::translate() sword code = " << config.translationCode;
	kDebug() << "a";
        char *cPos = pos.toLatin1().data();
        char *cCode = config.translationCode.toLatin1().data();

        VerseKey parser;
        ListKey result;

        result = parser.ParseVerseList(cPos, parser, true);
        /*  for (result = TOP; !result.Error(); result++) {
              qDebug() << result;
          }*/
        result.Persist(true);
        qDebug() << "verseDownloader::translate() sword getting module";
        target = library.getModule(cCode);
        if (target) {
            qDebug() << "verseDownloader::translate() sword set key cPos = " << cPos << " pos = " << pos;
            target->setKey(result);
            QString out = "";
            qDebug() << "verseDownloader::translate() sword out gen";
            out = QString::fromUtf8(target->RenderText());
            for (/*(*target) = TOP*/; !target->Error(); (*target)++) {
                if (target->RenderText() != NULL) {
                    out += QString::fromUtf8(target->RenderText());
                }
            }
            qDebug() << out;
            emit newVerse(out, pos);
        } else {
            emit newVerse("Can not load book!", "");
        }
        break;

    }

}
void verseDownloader::pharseTranslationsSite()
{
    QString out = downloadedData;
    qDebug() << "verseDownloader::pharseTranslationsSite() config.translationSource = " << config.translationSource;
    if (out.contains("Error:")) {
        qDebug() << "verseDownloader::pharseTranslationsSite() Error";
        emit newVerse(i18n("Error while loading verse. Please try an another translation."), "");
        return;
    }
    //pharse out
    QString text = " ", pos = " ", searchstring;
    if (config.translationSource == 1) {
        if (config.verseSource == 1) {
            QString bout = out;
            QString a = bout.remove(bout.indexOf("</a>)", 0), bout.size());
            a = a.remove("<div>");
            pos = a;
            text = a.remove(a.indexOf("(<a", 0), a.size());
            pos = pos.remove(0, pos.indexOf("\">", 0) + 2);
            emit newVerse(text, pos + "( from <a href=\"http://www.biblegateway.com\">biblegateway.com</a> )");
        } else {
            //first </sup>
            QString s1 = "</sup>";
            QString s2 = "</p><p /></div>";
            int pos1 = out.indexOf(s1, 0);
            int pos2 = out.indexOf(s2, 0);
            text = out;
            text = text.remove(pos2, out.size());
            text = text.remove(0, pos1);
            emit newVerse(text, pos + "( from <a href=\"http://www.biblegateway.com\">biblegateway.com</a> )");
        }
    }
    //emit newVerse(text,pos);
}
void verseDownloader::setConfig(struct configStruct newConfig)
{
    config = newConfig;
}
verseDownloader::~verseDownloader()
{

}
struct pos verseDownloader::convertPosition2Uni(QString pos, int from) {
    //qDebug() << "verseDownloader::convertPosition2Uni() from:"<< from <<" pos:" << pos;
    struct pos uPos;
    switch (from) {
    case 0://christnotes.org
    case 1://biblegateway.com
    case 2://sword
        QString bpos = pos;
        QString bpos2 = pos;
        int point = bpos.lastIndexOf(" ");
        QString bookName =  bpos2.remove(point, pos.size());
        QString rest =  bpos.remove(0, point);
        if (point == -1) {
            //qDebug() << "verseDownloader::convertPosition2Uni() pharse Error while step 1";
            return uPos;
        }

        QStringList list_2 = rest.split(":");
        if (list_2.size() < 2) {
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
        if (verse.contains("-")) {
            //qDebug() << "verseDownloader::convertPosition2Uni() more than one verse ( verse:"<< verse << ")";
            QStringList one = verse.split("-");
            uPos.verseID =  one.at(0).toInt();
            uPos.verseStartID = one.at(0).toInt();
            uPos.verseEndID = one.at(1).toInt();
            //qDebug() << "verseDownloader::convertPosition2Uni() verseStartID:"<<uPos.verseStartID << "verseeEndID"<<uPos.verseEndID;
        } else {
            uPos.verseID = verse.toInt();
            uPos.verseStartID = verse.toInt();
            uPos.verseEndID = verse.toInt();
        }
        return uPos;
        break;
    }
    return uPos;
}
QString verseDownloader::convertUni2Position(struct pos uPos, int to)
{
    //qDebug() << "verseDownloader::convertUni2Position to:" << to;
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
    QString bookName;
    QMapIterator<QString, int> i(bookMap);
    switch (to) {
    case 0://christnotes.org
    case 1://biblegateway.com
    case 2://sword
        while (i.hasNext()) {
            i.next();
            if (i.value() == uPos.bookID) {
                bookName = i.key();
                break;
            }
        }
        if (uPos.verseStartID != uPos.verseEndID) {
            //qDebug() << "verseDownloader::convertUni2Position() verseStartID = " << uPos.verseStartID << " verseEndID = " << uPos.verseEndID;
            qReturnString = bookName + " " + QString::number(uPos.chapterID) + ":" + QString::number(uPos.verseStartID) + "-" + QString::number(uPos.verseEndID);
            //qDebug() << "verseDownloader::convertUni2Position() more than one verse qReturnString = "<<qReturnString;
        } else {
            qReturnString = bookName + " " + QString::number(uPos.chapterID) + ":" + QString::number(uPos.verseID);
        }
        //() << "verseDownloader::convertUni2Position() qReturnString:"<<qReturnString;
        return qReturnString;
        break;
    }
    return "";
}