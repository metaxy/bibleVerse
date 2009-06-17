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
#include <QObject>
#include <QUrl>
#include <QHttp>
#include <QFile>
#include <QBuffer>
#include <iostream>
class webtool : public QObject
{
		Q_OBJECT
	public:
		webtool( QObject *parent = 0 );
		int get( const QUrl &url );
		int get( const QUrl &url , QString filename);
		int post( const QUrl &url, QString data );
		int post( const QUrl &url, QString data ,QString filename);
		void set_cookie( QString _cookie_ );
void set_referer( QString _referer_ );
		QString referer;
		static	QString useragent;
		static	QString accept;
		static	QString acceptlanguage;
		static	QString acceptcharset;
		static	QString acceptencoding;
		static	QString cookiea;
		static	QString useragents[11];
		static	QString useragents_w[11];
		QString out;
		QString status;
		QString cookie;

	signals:
		void sdone( QString dout, QString header );
	private slots:
		void read();
	private:
		bool nofile;
		QHttp http;
		QFile file;
		QBuffer* buffer;
};
