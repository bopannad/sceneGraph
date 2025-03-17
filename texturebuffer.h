#ifndef TEXTUREBUFFER_H
#define TEXTUREBUFFER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QSharedPointer>
#include <QMutex>

class QSGTexture;
class QQuickWindow;

class TextureBuffer : public QObject
{
    Q_OBJECT
public:
    static TextureBuffer& instance();
    
    QSGTexture* acquire(QQuickWindow* window, const QString& path);
    void releaseAll();
    void releaseTexture(const QString& path);
    bool contains(const QString& path) const;
    
private:
    struct TextureInfo {
        QSGTexture* texture = nullptr;
        QSharedPointer<QQuickWindow> window;
    };
    
    explicit TextureBuffer(QObject *parent = nullptr);
    ~TextureBuffer();
    
    static const int MAX_CACHE_SIZE = 100;
    QHash<QString, TextureInfo> m_textureCache;
    mutable QMutex m_mutex;
    
    void cleanupTexture(const QString& path);
    void limitCacheSize();
    bool isWindowValid(const QSharedPointer<QQuickWindow>& windowPtr) const;
};

#endif // TEXTUREBUFFER_H
