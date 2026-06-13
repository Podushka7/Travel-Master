#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QInputDialog> // Для ввода числа N

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , activeTourId(-1)
{
    ui->setupUi(this);

    // ПОДКЛЮЧАЕМ ДЕЙСТВИЯ ИЗ ФАЙЛА UI К СЛОТАМ
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    // Инициализация UI компонентов
    ui->comboType->addItems({"пляжный", "экскурсионный", "экзотический", "круиз", "VIP", "лечебный", "экстремальный", "горнолыжный"});
    ui->comboCountry->addItems({"Россия", "Абхазия", "Турция", "Египет"});
    ui->spinCost->setRange(3000, 100000);
    ui->spinDays->setRange(2, 31);

    Tour temp;
    ui->editPlace->setValidator(temp.createPlaceValidator(this));

    // Сигналы
    connect(ui->comboCountry, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_countryChanged);
    connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::saveButton_clicked);
    connect(ui->btnShow, &QPushButton::clicked, this, &MainWindow::showButton_clicked);
    connect(ui->btnAddRecord, &QPushButton::clicked, this, &MainWindow::addRecordButton_clicked);
    connect(ui->btnAdd10Records, &QPushButton::clicked, this, &MainWindow::add10RecordsButton_clicked);
    connect(ui->btnDeleteRecord, &QPushButton::clicked, this, &MainWindow::deleteRecordButton_clicked);
    connect(ui->listBrowser, &QListWidget::itemClicked, this, &MainWindow::on_browserItemClicked);

    // Создаем начальную базу (пустую или с 1 записью, по желанию)
    on_actionNew_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// === FILE OPERATIONS ===

void MainWindow::on_actionNew_triggered()
{
    if (!promptSaveIfNeeded()) return;

    db.clear();
    currentFileName.clear();

    // Создаем одну запись по умолчанию
    Tour t;
    t.resetToDefaults();
    db.append(t); // ID генерируется внутри
    activeTourId = t.id;

    fullBrowserUpdate();
    loadActiveTour();
    updateButtons();
}

void MainWindow::on_actionOpen_triggered()
{
    if (!promptSaveIfNeeded()) return;

    // Диалог открытия файла
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть БД", "", "Tour DB (*.tdb)");
    if (fileName.isEmpty()) return;

    if (db.load(fileName)) {
        currentFileName = fileName;
        // После загрузки активируем первую запись, если есть
        QVector<Tour> all = db.records();
        if (!all.isEmpty()) {
            activeTourId = all.first().id;
        } else {
            activeTourId = -1;
        }
        fullBrowserUpdate(); // Полное обновление только здесь
        loadActiveTour();
        updateButtons();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (currentFileName.isEmpty()) {
        on_actionSaveAs_triggered();
        return;
    }

    if (!db.save(currentFileName)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить БД как", "", "Tour DB (*.tdb)");
    if (fileName.isEmpty()) return;

    if (db.save(fileName)) {
        currentFileName = fileName;
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
    }
}

// Загрузка N первых записей
void MainWindow::on_actionLoadFirstN_triggered()
{
    // 1. Запрашиваем у пользователя количество записей N
    bool ok;
    int N = QInputDialog::getInt(this, "Загрузить N записей",
                                 "Введите количество записей для загрузки:",
                                 1, 1, 1000, 1, &ok);
    if (!ok || N <= 0) return; // Пользователь отменил ввод

    // 2. Выбираем файл для загрузки
    QString fileName = QFileDialog::getOpenFileName(this,
        "Выберите файл для загрузки записей", "", "Tour DB (*.tdb)");
    if (fileName.isEmpty()) return;

    // 3. Создаем временную базу для загрузки данных из файла
    TourDatabase tempDb;
    if (!tempDb.load(fileName)) {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось открыть файл для загрузки записей");
        return;
    }

    // 4. Получаем все записи из временной базы
    QVector<Tour> allRecords = tempDb.records();

    // 5. Определяем сколько записей реально можно загрузить
    int recordsToLoad = qMin(N, allRecords.size());

    if (recordsToLoad == 0) {
        QMessageBox::information(this, "Информация",
            "В выбранном файле нет записей для загрузки");
        return;
    }

    // 6. Загружаем N первых записей в основную базу
    int loadedCount = 0;
    for (int i = 0; i < recordsToLoad; ++i) {
        Tour record = allRecords[i];
        // Генерируем новый ID для избежания конфликтов
        record.id = -1; // Сброс ID для генерации нового
        db.append(record);
        addBrowserItem(record); // Точечное добавление в браузер
        loadedCount++;
    }

    // 7. Обновляем UI
    if (!db.records().isEmpty()) {
        activeTourId = db.records().last().id;
        loadActiveTour();
    }
    updateButtons();

    // 8. Информируем пользователя
    QMessageBox::information(this, "Загрузка завершена",
        QString("Успешно загружено %1 записей из файла\n"
                "Всего записей в базе: %2")
            .arg(loadedCount).arg(db.count()));
}

bool MainWindow::promptSaveIfNeeded()
{
    if (db.isModified()) {
        int ret = QMessageBox::warning(this, "Сохранение",
                                       "Данные были изменены. Сохранить?",
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes) {
            on_actionSave_triggered();
            return !db.isModified(); // Если сохранение удалось
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (promptSaveIfNeeded()) {
        event->accept();
    } else {
        event->ignore();
    }
}

// === RECORD OPERATIONS ===

void MainWindow::addRecordButton_clicked()
{
    Tour newTour;
    newTour.resetToDefaults();

    db.append(newTour);

    activeTourId = newTour.id;

    // Точечное обновление браузера
    addBrowserItem(newTour);

    loadActiveTour();
    updateButtons();
    QMessageBox::information(this, "Добавлено", "Новая запись добавлена!");
}

void MainWindow::saveButton_clicked()
{
    // Кнопка "Сохранить" в форме обновляет запись В ПАМЯТИ (Model)
    if (activeTourId == -1) return;

    Tour t;
    // Считываем текущую версию из базы, чтобы сохранить неизменяемые поля (id, createdAt)
    db.record(activeTourId, t);

    // Обновляем поля из UI
    t.saveFromUi(ui);

    // Обновляем в модели
    int pos = db.update(t);

    // Точечное обновление браузера
    updateBrowserItem(pos, t);

    QMessageBox::information(this, "Применено", "Запись обновлена в памяти. Не забудьте сохранить файл!");
}

void MainWindow::deleteRecordButton_clicked()
{
    if (db.count() <= 1) {
        QMessageBox::warning(this, "Ошибка", "Нельзя удалить последнюю запись!");
        return;
    }

    int ret = QMessageBox::question(this, "Удаление", "Удалить текущую запись?");
    if (ret == QMessageBox::Yes) {

        // --- ИСПРАВЛЕНИЕ НАЧАЛО ---
        // Ищем строку в браузере вручную, так как findItems ищет только по тексту
        int currentRow = -1;
        for (int i = 0; i < ui->listBrowser->count(); ++i) {
            QListWidgetItem* item = ui->listBrowser->item(i);
            if (item->data(Qt::UserRole).toInt() == activeTourId) {
                currentRow = i;
                break;
            }
        }
        // --- ИСПРАВЛЕНИЕ КОНЕЦ ---

        // Удаляем из модели
        db.remove(activeTourId);

        // Удаляем из браузера точечно
        if (currentRow != -1) {
            removeBrowserItem(currentRow);
        }

        // Выбираем новую активную запись
        QVector<Tour> all = db.records();
        if (!all.isEmpty()) {
            // Стараемся остаться на том же индексе или берем последний
            int newIndex = qMin(currentRow, all.size() - 1);
            if (newIndex < 0) newIndex = 0; // Защита
            activeTourId = all[newIndex].id;
        } else {
            activeTourId = -1;
        }

        loadActiveTour();
        updateButtons();
    }
}

void MainWindow::showButton_clicked()
{
    loadActiveTour(); // Просто перезагружаем данные из модели в UI (отмена правок)
}

// === BROWSER & UI LOGIC ===

void MainWindow::loadActiveTour()
{
    if (activeTourId == -1) return;

    Tour t;
    db.record(activeTourId, t);
    t.loadToUi(ui);
    on_countryChanged(ui->comboCountry->currentIndex());

    ui->labelRecord->setText(QString("ID записи: %1").arg(activeTourId));

    // Выделение в браузере
    for(int i=0; i<ui->listBrowser->count(); ++i) {
        if (ui->listBrowser->item(i)->data(Qt::UserRole).toInt() == activeTourId) {
            ui->listBrowser->setCurrentRow(i);
            break;
        }
    }
}

void MainWindow::updateButtons()
{
    ui->btnDeleteRecord->setEnabled(db.count() > 1);
    ui->labelRecordsCount->setText(QString("Всего записей: %1").arg(db.count()));
}

// Формирование строки для браузера
QString formatBrowserString(const Tour &t) {
    return QString("ID %1: %2 | %3 | %4 руб.")
            .arg(t.id).arg(t.type).arg(t.country).arg(t.cost);
}

void MainWindow::fullBrowserUpdate()
{
    ui->listBrowser->clear();
    QVector<Tour> records = db.records();
    for (const Tour &t : records) {
        QListWidgetItem *item = new QListWidgetItem(formatBrowserString(t), ui->listBrowser);
        item->setData(Qt::UserRole, t.id); // Храним ID в UserRole
    }
}

void MainWindow::addBrowserItem(const Tour &t)
{
    QListWidgetItem *item = new QListWidgetItem(formatBrowserString(t), ui->listBrowser);
    item->setData(Qt::UserRole, t.id);
    ui->listBrowser->scrollToItem(item);
}

void MainWindow::updateBrowserItem(int index, const Tour &t)
{
    if (index >= 0 && index < ui->listBrowser->count()) {
        QListWidgetItem *item = ui->listBrowser->item(index);
        item->setText(formatBrowserString(t));
        // ID не меняется
    }
}

void MainWindow::removeBrowserItem(int index)
{
    delete ui->listBrowser->takeItem(index);
}

void MainWindow::on_browserItemClicked(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();
    activeTourId = id;
    loadActiveTour();
}

void MainWindow::on_countryChanged(int index)
{
    QString country = ui->comboCountry->itemText(index);
    bool needsVisa = (country != "Россия");
    ui->checkVisa->setEnabled(needsVisa);
    ui->checkVisa->setChecked(needsVisa);

    ui -> listBrowser -> clear();

    for (const Tour &t : db.records())
    {
        if (t.country == country)
        {
            addBrowserItem(t);
        }
    }

}

// === GENERATOR ===

void MainWindow::add10RecordsButton_clicked()
{
    for (int i = 0; i < 10; ++i) {
        Tour t = createDiverseTour(0, i); // ID присвоится в append
        db.append(t);
        addBrowserItem(t); // Точечное добавление
    }

    QVector<Tour> all = db.records();
    if (!all.isEmpty()) activeTourId = all.last().id;

    loadActiveTour();
    updateButtons();
    QMessageBox::information(this, "Добавлено", "Добавлено 10 записей!");
}

Tour MainWindow::createDiverseTour(int, int index)
{
    Tour tour; // ID будет -1, но append его перезапишет
    QStringList types = {"пляжный", "экскурсионный", "экзотический", "круиз", "VIP", "лечебный", "экстремальный", "горнолыжный"};
    QStringList countries = {"Россия", "Абхазия", "Турция", "Египет"};
    QStringList places = {"Сочи", "Москва", "Санкт-Петербург", "Крым", "Алтай", "Адлер", "Анапа", "Геленджик", "Анталья", "Стамбул", "Шарм-эль-Шейх", "Хургада", "Гагра", "Пицунда"};
    QVector<int> costs = {15000, 25000, 35000, 45000, 55000, 65000, 75000, 85000, 95000, 100000};
    QVector<int> durations = {7, 10, 14, 21, 30};

    tour.type = types[index % types.size()];
    tour.country = countries[index % countries.size()];
    tour.place = places[index % places.size()];
    tour.cost = costs[index % costs.size()];
    tour.duration = durations[index % durations.size()];
    tour.visaRequired = (tour.country != "Россия" && tour.country != "Абхазия");

    return tour;
}


void MainWindow::on_saleButton_clicked()
{
    QVector<Tour> allTours = db.records();

    for (const Tour &t : allTours)
    {
        if (t.country == "Египет")
        {
            Tour updatedT = t;

            updatedT.cost = static_cast<int>(updatedT.cost * 0.9);

            db.update(updatedT);
        }
    }

    int currentIndex = ui -> comboCountry->currentIndex();
    on_countryChanged(currentIndex);

}


void MainWindow::Save(const QString &fileName, const QVector<Tour> &allTour)
{
    QFile file (fileName);

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Откройте файл для записи!");
        return;
    }

    QDataStream out(&file);

    for (const Tour &t : allTour)
    {
        out << t;
    }

    file.close();
}








