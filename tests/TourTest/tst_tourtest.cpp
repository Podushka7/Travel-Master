#include <QtTest>
#include "tour.h"
#include "tourdatabase.h"

class TourTest : public QObject
{
    Q_OBJECT

public:
    TourTest();
    ~TourTest();

private slots:
    void testFirst();
    void testOperators();
    void testDatabaseAppend();
    void testDatabaseRemove();
    void testTotalCostByCountry();
    void testSaleButton();
    void testValidatorPlace();
    void testSaveFile();
    void testDatabaseClear();
    void testParametricConstructor();
    void testCreatePlaceValidatorUsingValidator();
    void testDatabaseUpdate();
    void testRecordsOrder();
    void testIsModifiedFlag();
    // Тесты для Tour::resetToDefaults()
    void testResetToDefaultsType();
    void testResetToDefaultsCountry();
    void testResetToDefaultsPlace();
    void testResetToDefaultsCost();
    void testResetToDefaultsDuration();
    void testResetToDefaultsVisa();
    void testResetToDefaultsMultiple();
    void testResetToDefaultsAfterModify();
    // Тесты для Tour операторов
    void testOperatorsLessEqual();
    void testOperatorsGreaterEqual();
    void testOperatorsSameCost();
    void testOperatorsEqualDifferentDuration();
    void testOperatorsDifferentCost();
    void testOperatorsEdgeCases();
    // Тесты для TourDatabase::append()
    void testAppendMultipleRecords();
    void testAppendIdGeneration();
    void testAppendIdSequential();
    void testAppendModifiedFlag();
    void testAppendCountIncrement();
    void testAppendRecordsRetrievable();
    // Тесты для TourDatabase::totalCostByCountry()
    void testTotalCostByCountryEmptyDb();
    void testTotalCostByCountryNonExistent();
    void testTotalCostByCountrySingleRecord();
    void testTotalCostByCountryMultipleCountries();
    void testTotalCostByCountryZeroCost();
    void testTotalCostByCountryLargeCosts();
    // Тесты для TourDatabase::save() и load()
    void testSaveLoadEmpty();
    void testSaveLoadMultipleRecords();
    void testSaveLoadPreservesData();
    void testSaveLoadFileExists();
    void testLoadNonExistentFile();
    // Тесты для TourDatabase::records()
    void testRecordsEmpty();
    void testRecordsSingleRecord();
    void testRecordsMultiple();
    void testRecordsAfterRemove();
};

TourTest::TourTest() {}
TourTest::~TourTest() {}

void TourTest :: testFirst()
{
    Tour t;
    QCOMPARE(t.id, -1);
    QCOMPARE(t.country, QString("Россия"));
    QCOMPARE(t.cost, 3000);
}

void TourTest :: testOperators()
{
    Tour t1, t2;
    t1.duration = 7;
    t2.duration = 14;
    QVERIFY(t2<t1);
    QVERIFY(t1>t2);
    QVERIFY(t1==t2);
}

void TourTest :: testDatabaseAppend()
{
    TourDatabase db;
    QCOMPARE(db.count(), 0);

    Tour t;
    db.append(t);
    QCOMPARE(db.count(), 1);
    QCOMPARE(t.id, 1);
}

void TourTest :: testDatabaseRemove()
{
    TourDatabase db;
    Tour t1, t2;
    db.append(t1);
    db.append(t2);
    QCOMPARE(db.count(), 2);
    db.remove(1);
    QCOMPARE(db.count(), 1);
}

void TourTest :: testTotalCostByCountry()
{
    TourDatabase db;
    Tour t1, t2, t3;
    t1.country = "Турция";
    t1.cost = 50000;
    t2.country = "Турция";
    t2.cost = 50000;
    t3.country = "Россия";
    t3.cost = 30000;
    db.append(t1);
    db.append(t2);
    db.append(t3);

    QCOMPARE(db.totalCostByCountry("Турция"), 100000);
    QCOMPARE(db.totalCostByCountry("Россия"), 30000);
}

void TourTest :: testSaleButton()
{
    TourDatabase db;
    Tour t;
    t.country = "Египет"; t.cost = 10000;
    t.cost = static_cast<int>(t.cost * 0.9);
    db.append(t);
    Tour t1;
    db.record(t.id, t1);
    QCOMPARE(t1.cost, 9000);
}

void TourTest :: testValidatorPlace()
{
   QRegExp reg("^[A-Za-zА-Яа-яЁё\\s-]+$");
   QVERIFY(reg.exactMatch("Сочи"));
   QVERIFY(!reg.exactMatch("Сочи2024"));
   QVERIFY(reg.exactMatch("Санкт-Петербург"));
   QVERIFY(!reg.exactMatch("Зона 51"));
}

void TourTest::testSaveFile()
{
    QString testFile = "test_db.tdb";

    TourDatabase db;
    Tour t1;
    t1.country = "Италия";
    db.append(t1);
    QVERIFY(db.save(testFile));

    TourDatabase db2;
    QVERIFY(db2.load(testFile));
    QCOMPARE(db2.count(), 1);

    Tour checkedTour;
    db2.record(1, checkedTour);
    QCOMPARE(checkedTour.country, QString("Италия"));
    QFile::remove(testFile);
}

void TourTest :: testDatabaseClear()
{
    TourDatabase db;
    Tour t1, t2;
    db.append(t1);
    db.append(t2);
    QCOMPARE(t1.id, 1);
    QCOMPARE(db.count(), 2);
    db.clear();
    QCOMPARE(db.count(), 0);
}

void TourTest :: testParametricConstructor()
{
    Tour t(42);
    QCOMPARE(t.id, 42);
    QCOMPARE(t.place, "");

    Tour t1(-999);
    QCOMPARE(t1.id, -999);
}

void TourTest::testCreatePlaceValidatorUsingValidator()
{
    Tour t;
    QValidator *v = t.createPlaceValidator(this);
    int pos = 0;

    QString good = "Сочи";
    QVERIFY(v->validate(good, pos) == QValidator::Acceptable);

    QString bad = "Сочи2024";
    QVERIFY(v->validate(bad, pos) != QValidator::Acceptable);

    delete v;
}

void TourTest::testDatabaseUpdate()
{
    TourDatabase db;
    Tour t;
    db.append(t);

    t.place = "UpdatedPlace";
    t.cost = 12345;
    int pos = db.update(t);
    QVERIFY(pos >= 0);

    Tour out;
    db.record(t.id, out);
    QCOMPARE(out.place, QString("UpdatedPlace"));
    QCOMPARE(out.cost, 12345);
}

void TourTest::testRecordsOrder()
{
    TourDatabase db;
    Tour t1, t2, t3;
    db.append(t1);
    db.append(t2);
    db.append(t3);

    QVector<Tour> all = db.records();
    QCOMPARE(all.size(), 3);
    QCOMPARE(all[0].id, 1);
    QCOMPARE(all[1].id, 2);
    QCOMPARE(all[2].id, 3);
}

void TourTest::testIsModifiedFlag()
{
    QString filename = "test_mod.tdb";
    QFile::remove(filename);

    TourDatabase db;
    QVERIFY(!db.isModified());

    Tour t;
    db.append(t);
    QVERIFY(db.isModified());

    QVERIFY(db.save(filename));
    QVERIFY(!db.isModified());

    QFile::remove(filename);
}

// === Тесты для Tour::resetToDefaults() ===

void TourTest::testResetToDefaultsType()
{
    // Проверяет что resetToDefaults() устанавливает тип по умолчанию
    Tour t;
    t.type = "экзотический";
    t.resetToDefaults();
    QCOMPARE(t.type, QString("пляжный"));
}

void TourTest::testResetToDefaultsCountry()
{
    // Проверяет что resetToDefaults() устанавливает страну "Россия"
    Tour t;
    t.country = "Турция";
    t.resetToDefaults();
    QCOMPARE(t.country, QString("Россия"));
}

void TourTest::testResetToDefaultsPlace()
{
    // Проверяет что resetToDefaults() очищает место отдыха
    Tour t;
    t.place = "Москва";
    t.resetToDefaults();
    QCOMPARE(t.place, QString(""));
}

void TourTest::testResetToDefaultsCost()
{
    // Проверяет что resetToDefaults() устанавливает стоимость 3000
    Tour t;
    t.cost = 50000;
    t.resetToDefaults();
    QCOMPARE(t.cost, 3000);
}

void TourTest::testResetToDefaultsDuration()
{
    // Проверяет что resetToDefaults() устанавливает длительность 2 дня
    Tour t;
    t.duration = 30;
    t.resetToDefaults();
    QCOMPARE(t.duration, 2);
}

void TourTest::testResetToDefaultsVisa()
{
    // Проверяет что resetToDefaults() отключает флаг визы
    Tour t;
    t.visaRequired = true;
    t.resetToDefaults();
    QVERIFY(!t.visaRequired);
}

void TourTest::testResetToDefaultsMultiple()
{
    // Проверяет что resetToDefaults() сбрасывает ВСЕ поля одновременно
    Tour t;
    t.type = "экстремальный";
    t.country = "Египет";
    t.place = "Каир";
    t.cost = 99000;
    t.duration = 21;
    t.visaRequired = true;
    t.resetToDefaults();
    QCOMPARE(t.type, QString("пляжный"));
    QCOMPARE(t.country, QString("Россия"));
    QCOMPARE(t.place, QString(""));
    QCOMPARE(t.cost, 3000);
    QCOMPARE(t.duration, 2);
    QVERIFY(!t.visaRequired);
}

void TourTest::testResetToDefaultsAfterModify()
{
    // Проверяет что повторный resetToDefaults() работает корректно
    Tour t;
    t.cost = 10000;
    t.resetToDefaults();
    QCOMPARE(t.cost, 3000);
    t.cost = 20000;
    t.resetToDefaults();
    QCOMPARE(t.cost, 3000);
}

// === Тесты для Tour операторов ===

void TourTest::testOperatorsLessEqual()
{
    // Проверяет оператор < когда первый тур дешевле второго
    Tour t1, t2;
    t1.cost = 10000;
    t2.cost = 20000;
    QVERIFY(t1 < t2);
}

void TourTest::testOperatorsGreaterEqual()
{
    // Проверяет оператор > когда первый тур дороже второго
    Tour t1, t2;
    t1.cost = 30000;
    t2.cost = 15000;
    QVERIFY(t1 > t2);
}

void TourTest::testOperatorsSameCost()
{
    // Проверяет операторы < и > при одинаковой стоимости (сравнивает длительность в обратном порядке)
    Tour t1, t2;
    t1.cost = 25000;
    t2.cost = 25000;
    t1.duration = 5;
    t2.duration = 10;
    QVERIFY(t2 < t1);
    QVERIFY(t1 > t2);
}

void TourTest::testOperatorsEqualDifferentDuration()
{
    // Проверяет оператор == игнорирует длительность (проверяет только страну и место)
    Tour t1, t2;
    t1.country = "Турция";
    t1.place = "Анталья";
    t1.duration = 7;
    t2.country = "Турция";
    t2.place = "Анталья";
    t2.duration = 14;
    QVERIFY(t1 == t2);
}

void TourTest::testOperatorsDifferentCost()
{
    // Проверяет < и > при разных стоимостях (в край них значениях)
    Tour cheap, expensive;
    cheap.cost = 3000;
    expensive.cost = 100000;
    QVERIFY(cheap < expensive);
    QVERIFY(expensive > cheap);
}

void TourTest::testOperatorsEdgeCases()
{
    // Проверяет операторы при нулевых/граничных значениях
    Tour t1, t2;
    t1.cost = 0;
    t2.cost = 0;
    t1.duration = 1;
    t2.duration = 2;
    QVERIFY(t2 < t1);
}

// === Тесты для TourDatabase::append() ===

void TourTest::testAppendMultipleRecords()
{
    // Проверяет добавление нескольких записей подряд
    TourDatabase db;
    Tour t1, t2, t3;
    db.append(t1);
    db.append(t2);
    db.append(t3);
    QCOMPARE(db.count(), 3);
}

void TourTest::testAppendIdGeneration()
{
    // Проверяет что каждой записи при добавлении назначается уникальный ID
    TourDatabase db;
    Tour t1, t2, t3;
    db.append(t1);
    db.append(t2);
    db.append(t3);
    QVERIFY(t1.id != t2.id);
    QVERIFY(t2.id != t3.id);
    QVERIFY(t1.id != t3.id);
}

void TourTest::testAppendIdSequential()
{
    // Проверяет что ID генерируются последовательно (1, 2, 3, ...)
    TourDatabase db;
    Tour t1, t2, t3;
    db.append(t1);
    db.append(t2);
    db.append(t3);
    QCOMPARE(t1.id, 1);
    QCOMPARE(t2.id, 2);
    QCOMPARE(t3.id, 3);
}

void TourTest::testAppendModifiedFlag()
{
    // Проверяет что флаг isModified() становится true после append
    TourDatabase db;
    QVERIFY(!db.isModified());
    Tour t;
    db.append(t);
    QVERIFY(db.isModified());
}

void TourTest::testAppendCountIncrement()
{
    // Проверяет что count() увеличивается с каждым append
    TourDatabase db;
    QCOMPARE(db.count(), 0);
    Tour t1;
    db.append(t1);
    QCOMPARE(db.count(), 1);
    Tour t2;
    db.append(t2);
    QCOMPARE(db.count(), 2);
}

void TourTest::testAppendRecordsRetrievable()
{
    // Проверяет что добавленные записи можно получить методом record()
    TourDatabase db;
    Tour orig;
    orig.place = "TestPlace";
    orig.cost = 12345;
    db.append(orig);
    Tour retrieved;
    db.record(orig.id, retrieved);
    QCOMPARE(retrieved.place, QString("TestPlace"));
    QCOMPARE(retrieved.cost, 12345);
}


// === Тесты для TourDatabase::totalCostByCountry() ===

void TourTest::testTotalCostByCountryEmptyDb()
{
    // Проверяет totalCostByCountry() возвращает 0 для пустой базы
    TourDatabase db;
    QCOMPARE(db.totalCostByCountry("Россия"), 0);
}

void TourTest::testTotalCostByCountryNonExistent()
{
    // Проверяет totalCostByCountry() возвращает 0 для несуществующей страны
    TourDatabase db;
    Tour t;
    t.country = "Турция";
    t.cost = 50000;
    db.append(t);
    QCOMPARE(db.totalCostByCountry("Абхазия"), 0);
}

void TourTest::testTotalCostByCountrySingleRecord()
{
    // Проверяет totalCostByCountry() для одной записи
    TourDatabase db;
    Tour t;
    t.country = "Россия";
    t.cost = 5000;
    db.append(t);
    QCOMPARE(db.totalCostByCountry("Россия"), 5000);
}

void TourTest::testTotalCostByCountryMultipleCountries()
{
    // Проверяет totalCostByCountry() корректно разделяет по странам
    TourDatabase db;
    Tour t1, t2, t3, t4;
    t1.country = "Россия";
    t1.cost = 10000;
    t2.country = "Россия";
    t2.cost = 20000;
    t3.country = "Турция";
    t3.cost = 30000;
    t4.country = "Египет";
    t4.cost = 40000;
    db.append(t1);
    db.append(t2);
    db.append(t3);
    db.append(t4);
    QCOMPARE(db.totalCostByCountry("Россия"), 30000);
    QCOMPARE(db.totalCostByCountry("Турция"), 30000);
    QCOMPARE(db.totalCostByCountry("Египет"), 40000);
}

void TourTest::testTotalCostByCountryZeroCost()
{
    // Проверяет totalCostByCountry() когда стоимость = 0
    TourDatabase db;
    Tour t;
    t.country = "Абхазия";
    t.cost = 0;
    db.append(t);
    QCOMPARE(db.totalCostByCountry("Абхазия"), 0);
}

void TourTest::testTotalCostByCountryLargeCosts()
{
    // Проверяет totalCostByCountry() с большими суммами
    TourDatabase db;
    Tour t1, t2;
    t1.country = "Турция";
    t1.cost = 99999;
    t2.country = "Турция";
    t2.cost = 100000;
    db.append(t1);
    db.append(t2);
    QCOMPARE(db.totalCostByCountry("Турция"), 199999);
}


// === Тесты для TourDatabase::save() и load() ===

void TourTest::testSaveLoadEmpty()
{
    // Проверяет сохранение и загрузку пустой базы данных
    QString filename = "test_empty.tdb";
    QFile::remove(filename);
    TourDatabase db;
    QVERIFY(db.save(filename));
    TourDatabase db2;
    QVERIFY(db2.load(filename));
    QCOMPARE(db2.count(), 0);
    QFile::remove(filename);
}

void TourTest::testSaveLoadMultipleRecords()
{
    // Проверяет сохранение и загрузку базы с несколькими записями
    QString filename = "test_multi.tdb";
    QFile::remove(filename);
    TourDatabase db;
    Tour t1, t2, t3;
    t1.country = "Россия";
    t1.cost = 10000;
    t2.country = "Турция";
    t2.cost = 20000;
    t3.country = "Египет";
    t3.cost = 30000;
    db.append(t1);
    db.append(t2);
    db.append(t3);
    QVERIFY(db.save(filename));
    TourDatabase db2;
    QVERIFY(db2.load(filename));
    QCOMPARE(db2.count(), 3);
    QFile::remove(filename);
}

void TourTest::testSaveLoadPreservesData()
{
    // Проверяет что все данные записей сохраняются и загружаются корректно
    QString filename = "test_preserve.tdb";
    QFile::remove(filename);
    TourDatabase db;
    Tour orig;
    orig.type = "экзотический";
    orig.country = "Египет";
    orig.place = "Шарм-эль-Шейх";
    orig.cost = 85000;
    orig.duration = 14;
    orig.visaRequired = true;
    db.append(orig);
    db.save(filename);
    TourDatabase db2;
    db2.load(filename);
    Tour loaded;
    db2.record(orig.id, loaded);
    QCOMPARE(loaded.type, orig.type);
    QCOMPARE(loaded.country, orig.country);
    QCOMPARE(loaded.place, orig.place);
    QCOMPARE(loaded.cost, orig.cost);
    QCOMPARE(loaded.duration, orig.duration);
    QCOMPARE(loaded.visaRequired, orig.visaRequired);
    QFile::remove(filename);
}

void TourTest::testSaveLoadFileExists()
{
    // Проверяет что после save файл на диске существует
    QString filename = "test_exists.tdb";
    QFile::remove(filename);
    TourDatabase db;
    Tour t;
    db.append(t);
    db.save(filename);
    QVERIFY(QFile::exists(filename));
    QFile::remove(filename);
}

void TourTest::testLoadNonExistentFile()
{
    // Проверяет что load() возвращает false для несуществующего файла
    TourDatabase db;
    QVERIFY(!db.load("non_existent_file_12345.tdb"));
}

// === Тесты для TourDatabase::records() ===

void TourTest::testRecordsEmpty()
{
    // Проверяет что records() возвращает пустой вектор для пустой базы
    TourDatabase db;
    QVector<Tour> all = db.records();
    QCOMPARE(all.size(), 0);
}

void TourTest::testRecordsSingleRecord()
{
    // Проверяет что records() возвращает вектор с одной записью
    TourDatabase db;
    Tour t;
    db.append(t);
    QVector<Tour> all = db.records();
    QCOMPARE(all.size(), 1);
    QCOMPARE(all[0].id, 1);
}

void TourTest::testRecordsMultiple()
{
    // Проверяет что records() возвращает все добавленные записи в правильном порядке
    TourDatabase db;
    Tour t1, t2, t3;
    db.append(t1);
    db.append(t2);
    db.append(t3);
    QVector<Tour> all = db.records();
    QCOMPARE(all.size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(all[i].id, i + 1);
    }
}

void TourTest::testRecordsAfterRemove()
{
    // Проверяет что records() обновляется после удаления записи
    TourDatabase db;
    Tour t1, t2, t3;
    db.append(t1);
    db.append(t2);
    db.append(t3);
    db.remove(2);
    QVector<Tour> all = db.records();
    QCOMPARE(all.size(), 2);
    QCOMPARE(all[0].id, 1);
    QCOMPARE(all[1].id, 3);
}

QTEST_APPLESS_MAIN(TourTest)

#include "tst_tourtest.moc"
