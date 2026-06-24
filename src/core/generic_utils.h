#ifndef GENERIC_UTILS_H
#define GENERIC_UTILS_H
#include <QString>

struct orderInfo
{
    QString orderName;
    QString price;

    bool operator<(const orderInfo& other) const
    {
        if (orderName == other.orderName)
        {
            return price < other.price;
        }
        return orderName < other.orderName;
    }
};


#endif // GENERIC_UTILS_H

