#ifndef TOUR_H
#define TOUR_H

#include <QString>
#include <QValidator>
#include <QDataStream>
#include "ui_mainwindow.h"

class Tour
{
public:
    int id;             // уникальный идентификатор
    QString type;       // вид отдыха
    QString country;    // страна
    QString place;      // место отдыха
    int cost;           // стоимость
    int duration;       // длительность
    bool visaRequired;  // требуется ли виза
    qint64 createdAt;   // время создания
    bool operator<(const Tour &other) const;
    bool operator==(const Tour &other) const;

    Tour();
    Tour(int tourId);

    void loadToUi(Ui::MainWindow *ui) const;
    void saveFromUi(Ui::MainWindow *ui);
    void resetToDefaults();

    // валидатор для поля "место отдыха"
    QValidator* createPlaceValidator(QObject *parent) const;

    // Операторы для записи и чтения из файла
    friend QDataStream &operator<<(QDataStream &out, const Tour &tour);
    friend QDataStream &operator>>(QDataStream &in, Tour &tour);

};

#endif // TOUR_H
