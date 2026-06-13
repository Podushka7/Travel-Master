#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "tour.h"
#include "tourdatabase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override; // Для проверки сохранения при выходе

private slots:
    void on_countryChanged(int index);

    // Слоты кнопок редактирования
    void saveButton_clicked();   // "Сохранить" изменения в записи (память)
    void showButton_clicked();   // Отменить изменения в UI
    void addRecordButton_clicked();
    void deleteRecordButton_clicked();
    void add10RecordsButton_clicked();
    void on_saleButton_clicked();
    void Save(const QString &fileName, const QVector<Tour> &allTour);

    // Слоты браузера
    void on_browserItemClicked(QListWidgetItem *item);

    // Слоты меню Файл
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();      // Сохранить в текущий файл
    void on_actionSaveAs_triggered();    // Сохранить как...
    void on_actionLoadFirstN_triggered(); // Загрузить N первых записей из другого файла

private:
    Ui::MainWindow *ui;

    TourDatabase db; // Экземпляр модели

    QString currentFileName; // Имя текущего файла
    int activeTourId;        // ID текущей редактируемой записи

    void loadActiveTour();   // Загрузка из модели в UI
    void updateButtons();    // Обновление состояния кнопок

    // Работа с браузером
    void fullBrowserUpdate(); // Полное обновление (только при load/new)
    void updateBrowserItem(int index, const Tour &t); // Точечное обновление
    void addBrowserItem(const Tour &t); // Точечное добавление
    void removeBrowserItem(int index); // Точечное удаление

    bool promptSaveIfNeeded(); // Вопрос о сохранении

    Tour createDiverseTour(int id, int index); // Генератор данных
};

#endif // MAINWINDOW_H
