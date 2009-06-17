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
#ifndef bibleverse_H
#define bibleverse_H
#include <QString>
#include <KIcon>
#include <QString>
#include <QtCore/QObject>
#include <QPainter>
#include <QLabel>
#include <QRect>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsLinearLayout>
#include <Plasma/Applet>
#include <Plasma/Svg>
#include <Plasma/Label>
#include "config.h"
#include "ui_bibleVerseConfig.h"
class QSizeF;
// Define our plasma Applet
class PlasmaBibleVerse : public Plasma::Applet
{
		Q_OBJECT
	public:
		PlasmaBibleVerse(QObject *parent, const QVariantList &args);
		~PlasmaBibleVerse();
		void paintInterface(QPainter *painter,const QStyleOptionGraphicsItem *option,const QRect& contentsRect);
		void init();
		virtual QList<QAction*> contextualActions();

	public slots:
		void showVerse(QString text,QString pos);
		void configAccepted();
		void loadVerse();
	protected:
		void createConfigurationInterface(KConfigDialog *parent);
	private:
		
		void myPaint(QPainter *p,const QStyleOptionGraphicsItem *option, const QRect &contentsRect);
		QString out;
		Plasma::Svg m_svg;
		Plasma::Label *m_label;
		KIcon m_icon;
		QGraphicsLinearLayout *m_layout;
		Ui::bibleVerseConfig configUi;
		struct configStruct myConfig;
		QString lastDate;
		bool loading;
		QList<QAction *> actions;
		void createMenu();

};
K_EXPORT_PLASMA_APPLET(bibleverse, PlasmaBibleVerse)
#endif