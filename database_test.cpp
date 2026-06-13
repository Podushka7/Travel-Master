#include <QtTest>
#include "tourdatabase.h"
#include "tour.h"

class DatabaseTest : public QObject
{
    Q_OBJECT

private slots:
    void testEgyptSaleLogic();      // Паша тестирует горящую скидку для Египта
    void testSerializationAndId();  // Паша тестирует сохранение и инкремент ID
};

void DatabaseTest::testEgyptSaleLogic()
{
    TourDatabase db;
    Tour t1;
    t1.country = "Египет";
    t1.cost = 50000;
    db.append(t1); // Добавили Египет за 50к

    Tour t2;
    t2.country = "Россия";
    t2.cost = 30000;
    db.append(t2); // Добавили Россию за 30к

    // Паша имитирует вызов логики скидок (как при нажатии кнопки в UI)
    QVector<Tour> allTours = db.records();
    for (const Tour &t : allTours) {
        if (t.country == "Египет") {
            Tour updatedT = t;
            updatedT.cost = static_cast<int>(updatedT.cost * 0.9);
            db.update(updatedT);
        }
    }

    // БАЗА АССЕРТОВ ПАШИ:
    // 1. Проверяем, что Египет получил скидку 10% (50000 * 0.9 = 45000)
    QCOMPARE(db.records().at(0).cost, 45000);
    // 2. Проверяем, что стоимость тура в Россию осталась без изменений
    QCOMPARE(db.records().at(1).cost, 30000);
}

void DatabaseTest::testSerializationAndId()
{
    TourDatabase dbWrite;
    Tour t1;
    t1.type = "экскурсионный";
    t1.country = "Турция";
    t1.place = "Стамбул";
    t1.cost = 45000;

    // Метод append должен вернуть позицию в map и сгенерировать ID = 1
    int pos = dbWrite.append(t1);
    QCOMPARE(pos, 0); 
    QCOMPARE(dbWrite.records().first().id, 1);

    // Записываем в тестовый временной файл на диске
    QVERIFY(dbWrite.save("test_output.tdb") == true);

    // Создаем чистую базу для чтения
    TourDatabase dbRead;
    QVERIFY(dbRead.load("test_output.tdb") == true);

    // Проверяем, что данные восстановились идентично
    QCOMPARE(dbRead.count(), 1);
    QCOMPARE(dbRead.records().first().country, QString("Турция"));
    QCOMPARE(dbRead.records().first().place, QString("Стамбул"));
}

// Этот макрос автоматически генерирует отдельный main() для тестов
QTEST_APPLESS_MAIN(DatabaseTest)
#include "database_test.moc"
