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
#include "web.h"
#include <QString>
#include <QUrl>
#include <QHttp>
#include <QBuffer>
#include <QByteArray>
#include <QHttpResponseHeader>
#include <QtCore/QObject>
#include <QtDebug>
#include <QTextCodec>
web::web( QObject *parent ) : QObject( parent )
{
	QObject::connect( &http, SIGNAL( done( bool ) ), this, SLOT( read() ) );
}

int web::get( const QUrl &url )
{
	buffer = new QBuffer( this );
	buffer->open( QBuffer::ReadWrite );
	QString query( url.encodedQuery() );
	if ( query != "" )
	{
		query = "?" + query;
	}
	QHttpRequestHeader *header = new QHttpRequestHeader( "GET", url.path() + query );
	header->setValue( "Host", url.host() );
	header->setValue( "User-Agent", "BibleVerse Plasmoid ( KDE 4.2 Qt 4.5 ) " );
	header->setValue( "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
	header->setValue( "Accept-Language", "" );
	header->setValue( "Accept-Charset", "UTF-8,*" );
	header->setValue( "Keep-Alive", "300" );
	header->setValue( "Connection", "keep-alive" );

	if ( !url.isValid() )
	{
		qDebug() << "Error: Invalid URL" ;
		return false;
	}
	if ( url.scheme() != "http" )
	{
		qDebug() << "Error: URL must start with 'http:'";
		return false;
	}
	if ( url.path().isEmpty() )
	{
		qDebug() << "Error: URL has no path";
		return false;
	}
	http.setHost( url.host() );
	http.request(( *header ), "", buffer );
	return 1;
}

int web::post( const QUrl &url, QString dataa = "" )
{
	buffer = new QBuffer( this );
	buffer->open( QBuffer::ReadWrite );
	QString query( url.encodedQuery() );
	if ( query != "" )
	{
		query = "?" + query;
	}
	QHttpRequestHeader *header = new QHttpRequestHeader( "POST", url.path() + query );
	header->setValue( "Host", url.host() );
	header->setValue( "User-Agent", "BibleVerse Plasmoid ( KDE 4.2 Qt 4.5 ) " );
	header->setValue( "Accept-Charset", "UTF-8,*" );
	header->setValue( "Keep-Alive", "300" );
	header->setValue( "Connection", "keep-alive" );
	header->setValue( "Content-Type", "application/x-www-form-urlencoded" );
	if ( !url.isValid() )
	{
		qDebug() << "Error: Invalid URL";
		return false;
	}
	if ( url.scheme() != "http" )
	{
		qDebug() << "Error: URL must start with 'http:'" ;
		return false;
	}
	if ( url.path().isEmpty() )
	{
		qDebug() << "Error: URL has no path";
		return false;
	}
	QString data_(dataa);
	QByteArray data(data_. toAscii());
	http.setHost( url.host() );
	http.request(( *header ), data, buffer );
	return 1;
}

void web::read()
{
	buffer->close();
	QByteArray data;
	data = buffer->data();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextDecoder *decoder = codec->makeDecoder();
	QString out,status;
	out = decoder->toUnicode(data);
	status = http.lastResponse().statusCode();
	QHttpResponseHeader header = http.lastResponse();
	emit sdone( out, header.toString() );
}
#include "web.moc"

