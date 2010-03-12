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

#ifndef VERSEDOWNLOADER_H
#define VERSEDOWNLOADER_H
#include <QString>
#include <kio/scheduler.h>
#include <kurl.h>
#include <kio/jobclasses.h>
#include "config.h"
class verseDownloader : public QObject
{
    Q_OBJECT
public:
    verseDownloader(QObject *parent = 0);
    ~verseDownloader(void);
    void setConfig(struct configStruct newConfig);
    void downloadNew(void);
    enum DownloadSource
    {
        DownloadSourceChristnotes = 0,
        DownloadSourceBiblegateway = 1
    };
    enum TranslationSource
    {
        TranslationSourceNone = 0,
        TranslationSourceBiblegateway = 1
    };
signals:
    void newVerse(QString text, QString pos);
public slots:
    void pharseSourceSite(KJob *);
    void pharseTranslationsSite(KJob *);
    void downloaded(KIO::Job *job, const QByteArray &data);
private:
    void translate(QString text, QString pos);
    struct pos convertPosition2Uni(QString pos, int from);
    QString convertUni2Position(struct pos iPos, int to);
    bool translationNeeded;
    struct configStruct config;
    QString downloadedData;
    QString lastPos;
};

#endif // VERSEDOWNLOADER_H
