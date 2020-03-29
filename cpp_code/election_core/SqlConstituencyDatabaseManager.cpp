#include "SqlConstituencyDatabaseManager.h"

#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <Qvariant>

#include "Constituency.h"
#include "election_core_definitions.h"
#include "election_core_utils.h"
#include "Politician.h"

using namespace std;

namespace
{
unique_ptr<Constituency> sqlQueryToConstituency(const QSqlQuery& query);
}

SqlConstituencyDatabaseManager::SqlConstituencyDatabaseManager(
    const QFileInfo& databaseFileInfo,
    std::shared_ptr<DatabaseSignaller> databaseSignaller)
    : IConstituencyDatabaseManager(databaseSignaller)
    , databaseFileInfo_(databaseFileInfo)
{
    auto database =
        election_core_utils::connectToSqlDatabase(databaseFileInfo_);
    if (!database.isValid() || database.tables().contains("constituencies"))
        return;

    QSqlQuery query(database);
    query.exec(
        "CREATE TABLE constituencies "
        "("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT, "
        "latitude INTEGER, "
        "longitude INTEGER"
        ")");
}

SqlConstituencyDatabaseManager* SqlConstituencyDatabaseManager::clone() const
{
    return new SqlConstituencyDatabaseManager(*this);
}

void SqlConstituencyDatabaseManager::addConstituency(
    Constituency& constituency) const
{
    auto database =
        election_core_utils::connectToSqlDatabase(databaseFileInfo_);
    if (!database.isValid())
        return;

    QSqlQuery query(database);
    query.prepare(
        "INSERT INTO constituencies "
        "(name, latitude, longitude) "
        "VALUES "
        "(:name, :latitude, :longitude)");
    query.bindValue(":name", constituency.name());
    query.bindValue(":latitude", constituency.latitude());
    query.bindValue(":longitude", constituency.longitude());
    query.exec();
    constituency.setId(query.lastInsertId().toInt());
}

void SqlConstituencyDatabaseManager::updateConstituency(
    const Constituency& constituency) const
{
    auto database =
        election_core_utils::connectToSqlDatabase(databaseFileInfo_);
    if (!database.isValid())
        return;

    QSqlQuery query(database);
    query.prepare(
        "UPDATE constituencies SET "
        "name = (:name) "
        "latitude = (:latitude)"
        "longitude = (:longitude)"
        "WHERE "
        "id = (:id)");
    query.bindValue(":name", constituency.name());
    query.bindValue(":latitude", constituency.latitude());
    query.bindValue(":longitude", constituency.longitude());
    query.bindValue(":id", constituency.id());
    query.exec();
}

void SqlConstituencyDatabaseManager::removeConstituency(int id) const
{
    auto database =
        election_core_utils::connectToSqlDatabase(databaseFileInfo_);
    if (!database.isValid())
        return;

    QSqlQuery query(database);
    query.exec("DELETE FROM comstituencies WHERE id = " + QString::number(id));
}

unique_ptr<Constituency> SqlConstituencyDatabaseManager::constituency(
    int id) const
{
    auto database =
        election_core_utils::connectToSqlDatabase(databaseFileInfo_);
    if (!database.isValid())
        return nullptr;

    QSqlQuery query(database);
    query.exec(
        "SELECT * FROM comstituencies WHERE id = " + QString::number(id));
    return sqlQueryToConstituency(query);
}

vector<unique_ptr<Constituency>> SqlConstituencyDatabaseManager::
    constituencies() const
{
    auto database =
        election_core_utils::connectToSqlDatabase(databaseFileInfo_);
    if (!database.isValid())
        return vector<unique_ptr<Constituency>>();

    vector<unique_ptr<Constituency>> ret;
    QSqlQuery query(database);
    if (!query.exec("SELECT * FROM constituencies"))
        return vector<unique_ptr<Constituency>>();
    while (query.next()) ret.push_back(sqlQueryToConstituency(query));

    return ret;
}

bool SqlConstituencyDatabaseManager::refreshDatabase() const
{
    if (python_scripting::runPythonScript(
            QFileInfo(paths::constituencyScrapingScript)))
    {
        if (auto signaller = databaseSignaller())
            emit signaller->databaseRefreshed();
        return true;
    }

    return false;
}

namespace
{
unique_ptr<Constituency> sqlQueryToConstituency(const QSqlQuery& query)
{
    unique_ptr<Constituency> constituency(new Constituency);
    constituency->setId(query.value("id").toInt());
    constituency->setName(query.value("name").toString());
    constituency->setLatitude(query.value("latitude").toInt());
    constituency->setLongitude(query.value("longitude").toInt());
    return constituency;
}
}// namespace