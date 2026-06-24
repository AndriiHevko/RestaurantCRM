#ifndef SERVER_USER_H
#define SERVER_USER_H
#include <QString>
#include <map>
#include "generic_utils.h"

struct MenuItem
{
    QString itemName;
    QString description;
    QString price;
    QString weight;
    QString category;
    QString isAvailable;
};

class ServerUser
{
public:
    ServerUser();

    static QVector<MenuItem> fetchMenuItems(const QString& matchingPattern);

    static QString getUserId(const QString& userLogin);
    static bool insertOrders(const QString& userId, const std::map<orderInfo, int>& orders);
};

#endif // SERVER_USER_H
