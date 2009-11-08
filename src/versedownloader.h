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
signals:
    void newVerse(QString text, QString pos);
public slots:
    void pharseSourceSite();
    void pharseTranslationsSite();
    void downloaded(KIO::Job *job, const QByteArray &data);
private:
    void translate(QString text, QString pos);
    struct pos convertPosition2Uni(QString pos, int from);
    QString convertUni2Position(struct pos iPos, int to);
    bool translationNeeded;
    struct configStruct config;
    QString downloadedData;


};

#endif // VERSEDOWNLOADER_H
