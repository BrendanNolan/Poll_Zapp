#ifndef CONSTITUENCY_H
#define CONSTITUENCY_H

#include <QUrl>
#include <QString>

#include "election_core_global.h"

#include "MP.h"

class ELECTIONCORESHARED_EXPORT Constituency
{
public:
    Constituency() = default;
    explicit Constituency(
        const QString& name,
        int latitude,
        int longitude,
        const QUrl& pictoralRepresentationUrl);

    int id() const;
    void setId(int id);
    QString name() const;
    void setName(const QString& name);
    QUrl pictoralRepresentation() const;
    void setPictoralRepresentation(const QUrl& pic);
    int latitude() const;
    void setLatitude(int latitude);
    int longitude() const;
    void setLongitude(int longitude);
    const MP& mp() const;
    void setMp(const MP& mp);
    bool isValid() const;

private:
    int id_ = -1;
    QString name_ = QString("");
    int latitude_ = -1;
    int longitude_ = -1;
    QUrl pictoralRepresentationUrl_;
    MP mp_;
};

#endif // CONSTITUENCY_H
