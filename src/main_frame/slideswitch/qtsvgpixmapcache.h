#ifndef QT_SVG_PIXMAP_CACHE
#define QT_SVG_PIXMAP_CACHE

#include <QtCore/QSizeF>
#include <QObject>


class QPainter;
class QString;
class QRectF;

class QtSvgPixmapCachePrivate;

class QtSvgPixmapCache : public QObject
{
public:
    QtSvgPixmapCache(QObject* parent = NULL);
    QtSvgPixmapCache(const QString& url);
    ~QtSvgPixmapCache();

    bool load(const QString& url);
    bool isValid() const;

    void render(QPainter* painter, const QRectF& bounds);
    QSize defaultSize() const;

private:
    Q_DISABLE_COPY(QtSvgPixmapCache)
    QtSvgPixmapCachePrivate * const d;
};


#endif // QTSVGPIXMAPCACHE_H
