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
#include "web.h"

#include <QUrl>
#include <QHttp>
#include <QBuffer>
#include <QByteArray>
#include <QHttpResponseHeader>

using namespace std;
webtool::webtool( QObject *parent ) : QObject( parent )
{
	QObject::connect( &http, SIGNAL( done( bool ) ), this, SLOT( read() ) );
}

int webtool::get( const QUrl &url )
{
	nofile = true;
	buffer = new QBuffer( this );
	buffer->open( QBuffer::ReadWrite );
	QString query( url.encodedQuery() );
	if ( query != "" )
	{
		query = "?" + query;
	}
	QHttpRequestHeader *header = new QHttpRequestHeader( "GET", url.path() + query );
	header->setValue( "Host", url.host() );
	header->setValue( "User-Agent", "Qt4.5 BibleVerse Plasmoid" );
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

int webtool::post( const QUrl &url, QString dataa = "" )
{
	nofile = true;
	buffer = new QBuffer( this );
	buffer->open( QBuffer::ReadWrite );
	QString query( url.encodedQuery() );
	if ( query != "" )
	{
		query = "?" + query;
	}
	QHttpRequestHeader *header = new QHttpRequestHeader( "POST", url.path() + query );
	header->setValue( "Host", url.host() );
	header->setValue( "User-Agent", "Qt4.5 BibleVerse Plasmoid" );
	header->setValue( "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
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
	QByteArray data = dataa.toStdString().c_str();
	http.setHost( url.host() );
	http.request(( *header ), data, buffer );
	return 1;
}

void webtool::read()
{
	if ( nofile == false )
	{
		file.close();
		emit sdone( "", "" );
	}
	else
	{
		buffer->close();
		QByteArray data;
		data = buffer->data();
		QString out_1( data );
		out = out_1;
		status = http.lastResponse().statusCode();
		QHttpResponseHeader header = http.lastResponse();
		emit sdone( out, header.toString() );
	}

}

