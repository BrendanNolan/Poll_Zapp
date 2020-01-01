#include "SqlPoliticianDatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>
#include <QVariant>

#include "Politician.h"

using namespace std;

namespace
{
    unique_ptr<Politician> sqlQueryToPolitician(const QSqlQuery& query);
}

SqlPoliticianDatabaseManager::SqlPoliticianDatabaseManager(
    shared_ptr<QSqlDatabase> database)
    : database_(move(database))
{
    if (database_->tables().contains("politicians"))
        return;
    QSqlQuery query(*database_);
    query.exec(
        "CREATE TABLE politicians "
        "("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "constituency_id INTEGER, "
        "image_url TEXT, "
        "name TEXT, "
        "elected INTEGER, " // 0 for false 1 for true
        "candidate INTEGER, " // 0 for false 1 for true
        "party_name TEXT, "
        "party_rgb_red TEXT, "
        "party_rgb_green TEXT, "
        "party_rgb_blue TEXT"
        ")");
}

vector<unique_ptr<Politician>> SqlPoliticianDatabaseManager::mpsForConstituency(
    int constituencyId) const
{
    vector<unique_ptr<Politician>> ret;
    if (!database_)
        return ret;

    QSqlQuery query(*database_);
    query.prepare(
        "SELECT * FROM politicians "
        "WHERE "
        "("
        "constituency_id = (:constituency_id) "
        "AND "
        "elected = (:elected)"
        ")");
    query.bindValue(":constituency_id", constituencyId);
    query.bindValue(":elected", 1);
    query.exec();
    while (query.next())
        ret.push_back(sqlQueryToPolitician(query));
    return ret;
}

vector<unique_ptr<Politician>> 
SqlPoliticianDatabaseManager::candidatesForConstituency(
    int constituencyId) const
{
    vector<unique_ptr<Politician>> ret;
    if (!database_)
        return ret;
    QSqlQuery query(*database_);
    query.prepare(
        "SELECT * FROM politicians "
        "WHERE "
        "("
        "constituency_id = (:constituency_id) "
        "AND "
        "candidate = (:candidate)"
        ")");
    query.bindValue(":constituency_id", constituencyId);
    query.bindValue(":candidate", 1);
    query.exec();
    while (query.next())
        ret.push_back(sqlQueryToPolitician(query));
    return ret;
}

unique_ptr<Politician> SqlPoliticianDatabaseManager::politician(int id) const
{
    QSqlQuery query(*database_);
    query.exec("SELECT * FROM politicians WHERE id = " + QString::number(id));
    return sqlQueryToPolitician(query);
}

QUrl SqlPoliticianDatabaseManager::imageUrlForPolitician(int politicianId) const
{
    QSqlQuery query(*database_);
    query.exec(
        "SELECT image_url FROM politicians WHERE id = " 
        + QString::number(politicianId));
    return QUrl(query.value("image_url").toString());
}

void SqlPoliticianDatabaseManager::addPoliticianToConstituency(
    Politician& thePolitician,
    int constituencyId) const
{
    thePolitician.setConstituencyId(constituencyId);

    QSqlQuery query(*database_);
    query.prepare(
        "INSERT INTO politicians "
        "("
        "constituency_id, image_url, name, elected, candidate, "
        "party_name, party_rgb_red, party_rgb_green, party_rgb_blue"
        ") "
        "VALUES "
        "("
        ":constituency_id, :image_url, :name, :elected, :candidate, "
        ":party_name, :party_rgb_red, :party_rgb_green, :party_rgb_blue"
        ")");
    query.bindValue(":constituency_id", thePolitician.constituencyId());
    query.bindValue(":image_url", thePolitician.imageUrl().toString());
    query.bindValue(":name", thePolitician.name());
    query.bindValue(":elected", static_cast<int>(thePolitician.elected()));
    query.bindValue(":candidate", static_cast<int>(thePolitician.candidate()));
    query.bindValue(":party_name", thePolitician.partyDetails().name_);
    query.bindValue(
        ":party_rgb_red", 
        thePolitician.partyDetails().colour_.red_);
    query.bindValue(
        ":party_rgb_green", 
        thePolitician.partyDetails().colour_.green_);
    query.bindValue(
        ":party_rgb_blue", 
        thePolitician.partyDetails().colour_.blue_);
    thePolitician.setId(query.lastInsertId().toInt());
}

void SqlPoliticianDatabaseManager::updatePolitician(
    const Politician & politician) const
{
    if (!database_)
        return;

    QSqlQuery query(*database_);
    query.prepare(
        "UPDATE politicians SET "
        "("
        "image_url = (:image_url), "
        "name = (:name), "
        "constituency_id = (:constituency_id), "
        "party_rgb_red = (:party_rgb_red), "
        "party_rgb_green = (:party_rgb_green), "
        "party_rgb_blue = (:party_rgb_blue), "
        "party_name = (:party_name), "
        "elected = (:elected), "
        "candidate = (:candidate)"
        ") "
        "WHERE id = (:id)");
    query.bindValue(":image_url", politician.imageUrl());
    query.bindValue(":name", politician.name());
    query.bindValue(":constituency_id", politician.constituencyId());
    query.bindValue(":party_rgb_red", politician.partyDetails().colour_.red_);
    query.bindValue(
        ":party_rgb_green", 
        politician.partyDetails().colour_.green_);
    query.bindValue(":party_rgb_blue", politician.partyDetails().colour_.blue_);
    query.bindValue(":party_name", politician.partyDetails().name_);
    query.bindValue(":elected", static_cast<int>(politician.elected()));
    query.bindValue(":candidate", static_cast<int>(politician.candidate()));
    query.bindValue(":id", politician.id());
    query.exec();
}

void SqlPoliticianDatabaseManager::removePolitician(int politicianId) const
{
    if (!database_)
        return;

    QSqlQuery query(*database_);
    query.exec(
        "DELETE FROM politicians WHERE id = " + QString::number(politicianId));
}

void SqlPoliticianDatabaseManager::clearPoliticiansFromConstituency(
    int constituencyId) const
{
    if (!database_)
        return;

    QSqlQuery query(*database_);
    query.exec(
        "DELETE FROM politicians WHERE constituency_id = " 
        + QString::number(constituencyId));
}

namespace 
{
    unique_ptr<Politician> sqlQueryToPolitician(const QSqlQuery& query)
    {
        PartyDetails details;
        {
            RGBValue rgb(
                query.value("party_rgb_red").toInt(),
                query.value("party_rgb_green").toInt(),
                query.value("party_rgb_blue").toInt());
            details.name_ = query.value("party_name").toString();
            details.colour_ = rgb;
        }
        unique_ptr<Politician> politician(new Politician);
        politician->setImageUrl(QUrl(query.value("image_url").toString()));
        politician->setName(query.value("name").toString());
        politician->setId(query.value("id").toInt());
        politician->setConstituencyId(query.value("constituency_id").toInt());
        politician->setElected(query.value("elected").toInt());
        politician->setCandidate(query.value("candidate").toInt());
        politician->setPartyDetails(details);
        return politician;
    }
}