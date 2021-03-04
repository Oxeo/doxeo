#ifndef CAMERA_H
#define CAMERA_H

#include <QString>
#include <QMap>
#include <QJsonObject>

class Camera
{

public:
    Camera();
    Camera(int id);

    int getId()  const;
    QJsonObject toJson() const;
    bool flush();
    bool remove();

    static void update();
    static bool isIdValid(int id);
    static Camera *get(int id);
    static QMap<int, Camera*> getCameraList();

    QString getName() const;
    void setName(const QString &value);

    QString getUrl() const;
    void setUrl(const QString &value);

    int getOrder() const;
    void setOrder(int value);

    QString getVisibility() const;
    void setVisibility(const QString &value);

protected:
    int id;
    QString name;
    QString url;
    int order;
    QString visibility;

    static QMap<int, Camera*> cameraList;
};

#endif // CAMERA_H
