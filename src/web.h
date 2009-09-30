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
#ifndef WEB_H
#define WEB_H
#include <QObject>
#include <QUrl>
#include <QHttp>
#include <QBuffer>
#include <QString>
#include <iostream>
class web : public QObject
{
		Q_OBJECT
	public:
		web( QObject *parent = 0 );
		int get( const QUrl &url );
		int post( const QUrl &url, QString data );

	signals:
		void sdone( QString dout, QString header );
	private slots:
		void read();
	private:
		QHttp http;
		QBuffer* buffer;
};
#endif // WEb_H