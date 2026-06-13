#include "tour.h"
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDateTime>

Tour::Tour()
    : id(-1), type("пляжный"), country("Россия"), place("Анапа"),
      cost(3000), duration(2), visaRequired(false),
      createdAt(QDateTime::currentMSecsSinceEpoch())
{}

Tour::Tour(int tourId)
    : id(tourId), type("пляжный"), country("Россия"), place(""),
      cost(3000), duration(2), visaRequired(false),
      createdAt(QDateTime::currentMSecsSinceEpoch())
{}

void Tour::resetToDefaults()
{
    type = "пляжный";
    country = "Россия";
    place = "";
    cost = 3000;
    duration = 2;
    visaRequired = false;
    // createdAt не сбрасываем
}

void Tour::loadToUi(Ui::MainWindow *ui) const
{
    ui->comboType->setCurrentText(type);
    ui->comboCountry->setCurrentText(country);
    ui->editPlace->setText(place);
    ui->spinCost->setValue(cost);
    ui->spinDays->setValue(duration);
    ui->checkVisa->setChecked(visaRequired);
}

void Tour::saveFromUi(Ui::MainWindow *ui)
{
    type = ui->comboType->currentText();
    country = ui->comboCountry->currentText();
    place = ui->editPlace->text();
    cost = ui->spinCost->value();
    duration = ui->spinDays->value();
    visaRequired = ui->checkVisa->isChecked();
}

QValidator* Tour::createPlaceValidator(QObject *parent) const
{
    QRegularExpression reg("^[А-ЯЁA-Z][а-яёa-z\\s-]{0,49}$");
    return new QRegularExpressionValidator(reg, parent);
}

// Реализация сериализации для сохранения в файл
QDataStream &operator<<(QDataStream &out, const Tour &tour)
{
    out << tour.id << tour.type << tour.country << tour.place
        << tour.cost << tour.duration << tour.visaRequired << tour.createdAt;
    return out;
}

QDataStream &operator>>(QDataStream &in, Tour &tour)
{
    in >> tour.id >> tour.type >> tour.country >> tour.place
       >> tour.cost >> tour.duration >> tour.visaRequired >> tour.createdAt;
    return in;
}

bool Tour::operator<(const Tour &other) const
{
    if (this -> cost != other.cost)
    {
        return this -> cost < other.cost;
    }
    else
    {
        return this -> duration > other.duration;
    }

    return false;
}

bool Tour::operator==(const Tour &other) const
{
    return (this -> country == other.country) && (this -> place == other.place);
}
