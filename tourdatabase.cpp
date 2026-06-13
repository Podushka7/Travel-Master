#include "tourdatabase.h"
#include <QFile>
#include <QDataStream>
#include <QMapIterator>

TourDatabase::TourDatabase()
    : m_isModified(false), m_nextId(1)
{
}

int TourDatabase::count() const
{
    return m_map.size();
}

// Добавить запись. Генерирует ID и возвращает позицию.
int TourDatabase::append(Tour &record)
{
    record.id = m_nextId++; // Присвоение ID
    m_map.insert(record.id, record);
    m_isModified = true;
    return getPositionById(record.id); // Возвращаем позицию сортировки
}

// Удалить запись по ID
void TourDatabase::remove(unsigned int id)
{
    if (m_map.remove(id) > 0) {
        m_isModified = true;
    }
}

// Обновить запись. Возвращает позицию.
int TourDatabase::update(const Tour &record)
{
    if (m_map.contains(record.id)) {
        m_map[record.id] = record;
        m_isModified = true;
        return getPositionById(record.id);
    }
    return -1;
}

// Получить одну запись (только чтение)
void TourDatabase::record(unsigned int id, Tour &record) const
{
    if (m_map.contains(id)) {
        record = m_map.value(id);
    }
}

// Получить вектор всех записей для браузера
const QVector<Tour> TourDatabase::records() const
{
    QVector<Tour> result;
    // Использование Java-итератора для обхода
    QMapIterator<int, Tour> i(m_map);
    while (i.hasNext()) {
        i.next();
        result.append(i.value());
    }
    return result;
}

QMap<QString, QVector<Tour>> groupByCountry(const QVector<Tour> &allTours)
{
    QMap<QString, QVector<Tour>> groups;

    for (const Tour &t : allTours)
    {
        groups[t.country].append(t);
    }

    return groups;
}

// Сохранение в файл
bool TourDatabase::save(QString filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    QDataStream out(&file);
    // Записываем генератор ID, чтобы не было коллизий при перезапуске
    out << m_nextId;

    out << static_cast<quint32>(m_map.size());

    QMapIterator<int, Tour> i(m_map);
    while (i.hasNext()) {
        i.next();
        out << i.value();
    }

    // Сбрас флага modified происходит в const методе, поэтому m_isModified должен быть mutable
    m_isModified = false;
    return true;
}

// Загрузка из файла
bool TourDatabase::load(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    clear(); // Предыдущие данные уничтожаются

    QDataStream in(&file);
    in >> m_nextId; // Восстанавливаем счетчик ID

    quint32 size;
    in >> size;

    for (quint32 i = 0; i < size; ++i) {
        Tour t;
        in >> t;
        m_map.insert(t.id, t);
    }

    m_isModified = false;
    return true;
}

void TourDatabase::clear()
{
    m_map.clear();
    m_nextId = 1;
    m_isModified = false;
}

bool TourDatabase::isModified() const
{
    return m_isModified;
}

// Вспомогательный метод: найти индекс ключа в отсортированной map
int TourDatabase::getPositionById(int id) const
{
    // QMap отсортирован по ключам. Найдем порядковый номер.
    int position = 0;
    QMapIterator<int, Tour> i(m_map);
    while (i.hasNext()) {
        i.next();
        if (i.key() == id) return position;
        position++;
    }
    return -1;
}

int TourDatabase::totalCostByCountry(const QString &country) const
{
    int totalCost = 0;

    // Создаем Java-итератор для обхода контейнера m_map
    QMapIterator<int, Tour> i(m_map);

    while (i.hasNext()) {
        i.next(); // Переходим к следующему элементу

        // i.value() возвращает текущий объект Tour
        if (i.value().country == country) {
            totalCost += i.value().cost; // Суммируем стоимость
        }
    }

    return totalCost;
}

