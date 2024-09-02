#ifndef SERVERMAP_HPP
#define SERVERMAP_HPP

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>

template<typename T>
class ServerMap : public QMap<QString, T>
{
public:
    inline QJsonObject toJson(){
        QJsonObject mapObj;
        for(auto &&[server, list] : QMap<QString, T>::asKeyValueRange()){
            QJsonArray serverArr;
            for(auto &&account : list){
                serverArr.append(account.toJson());
            }
            mapObj.insert(server, serverArr);
        }
        return mapObj;
    }
};

#endif // SERVERMAP_HPP
