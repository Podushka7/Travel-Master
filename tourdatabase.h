#ifndef TOURDATABASE_H
#define TOURDATABASE_H

#include <QMap>
#include <QVector>
#include <QString>
#include "tour.h"

// Класс модели данных
class TourDatabase
{
public:
    TourDatabase();

    // Интерфейс модели согласно заданию:
    int count() const;
    int append(Tour &record);
    void remove(unsigned int id);
    int update(const Tour &record);
    void record(unsigned int id, Tour &record) const;
    const QVector<Tour> records() const;
    const QVector<Tour> countrySort() const;
    int totalCostByCountry(const QString &country) const;

    bool save(QString filename) const;
    bool load(QString filename);
    void clear();
    bool isModified() const;

private:
    QMap<int, Tour> m_map; // Контейнер QMap
    mutable bool m_isModified; // Флаг изменений
    int m_nextId; // Генератор ID

    // Внутренний метод для поиска позиции (индекса) по ID в QMap
    int getPositionById(int id) const;
};

#endif
