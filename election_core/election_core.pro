QT += sql

TARGET = election_core
TEMPLATE = lib
CONFIG += static

# DEFINES += ELECTIONCORE_LIBRARY

SOURCES += \
    SqlConstituencyDatabaseManager.cpp \
    Constituency.cpp \
    ConstituencyModel.cpp \
    PoliticianModel.cpp \
    PollResultModel.cpp \
    IPollResultDatabaseManager.cpp \
    IPoliticianDatabaseManager.cpp \
    IConstituencyDatabaseManager.cpp \
    SqlDatabaseManagerFactory.cpp \
    Politician.cpp \
    PollResult.cpp \
    SqlPollResultDatabaseManager.cpp \
    SqlPoliticianDatabaseManager.cpp \
    election_core_utils.cpp

HEADERS += \
    IConstituencyDatabaseManager.h \
    SqlConstituencyDatabaseManager.h \
    Constituency.h \
    ConstituencyModel.h \
    PoliticianModel.h \
    PollResultModel.h \
    IPollResultDatabaseManager.h \
    IPoliticianDatabaseManager.h \
    IDatabaseManagerFactory.h \
    SqlDatabaseManagerFactory.h \
    Politician.h \
    PollResult.h \
    SqlPollResultDatabaseManager.h \
    SqlPoliticianDatabaseManager.h \
    election_core_utils.h \
    UnaryPredicate.h \
    election_core_definitions.h \
    DatabaseSignaller.h

INCLUDEPATH += \
    $$(PYTHON_38_STUFF)\include
