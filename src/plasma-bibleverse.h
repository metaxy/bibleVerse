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
#ifndef bibleverse_H
#define bibleverse_H
#include <QString>
#include <KIcon>
#include <QString>
#include <QtCore/QObject>
#include <QPainter>
#include <QLabel>
#include <QRect>
#include <QtCore/QTimer>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsLinearLayout>
#include <Plasma/Applet>
#include <Plasma/Svg>
#include <Plasma/Label>
#include "config.h"
#include "ui_bibleVerseGeneralConfig.h"
#include "ui_bibleVerseViewConfig.h"
#include "versedownloader.h"
class QSizeF;
// Define our plasma Applet
class PlasmaBibleVerse : public Plasma::Applet
{
    Q_OBJECT
public:
    PlasmaBibleVerse(QObject *parent, const QVariantList &args);
    ~PlasmaBibleVerse();
    void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect& contentsRect);
    void init();
    virtual QList<QAction*> contextualActions();

public slots:
    void showVerse(QString text, QString pos);
    void configAccepted();
    void loadVerse();
    void translationConfig(int index);
protected:
    void createConfigurationInterface(KConfigDialog *parent);
private:
    QStringList translationCode;
    QString out;
    Plasma::Svg m_svg;
    Plasma::Label *m_label;
    KIcon m_icon;
    QGraphicsLinearLayout *m_layout;
    Ui::bibleVerseGeneralConfig generalConfigUi;
    Ui::bibleVerseViewConfig viewConfigUi;
    verseDownloader *vdownloader;
    struct configStruct myConfig;
    QString lastDate;
    bool loading;
    QList<QAction *> actions;
    void createMenu();
    QTimer *timer;

};
K_EXPORT_PLASMA_APPLET(bibleverse, PlasmaBibleVerse)
#endif