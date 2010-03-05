#ifndef CONFIG_H
#define CONFIG_H
#include <QtCore/QString>
struct configStruct {
    bool showPosition;
    int verseSource;
    int translationSource;
    QString translationCode;
    QString fontSize;
    QString fontColor;
    int autoUpdate;
};
#endif // CONFIG_H
