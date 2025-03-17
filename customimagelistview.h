#ifndef CUSTOMIMAGELISTVIEW_H
#define CUSTOMIMAGELISTVIEW_H

#include <QQuickItem>
#include <QList>
#include <QUrl>
#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QMap>
#include <QSGGeometryNode>
#include <QImage>
#include <QSGSimpleTextureNode>
#include <QSGTextureMaterial>
#include <QSGOpaqueTextureMaterial>
#include <QSGFlatColorMaterial>
#include <QQueue>  // Add this include
#include "texturebuffer.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkConfiguration>
#include <QSslError>
#include <QPropertyAnimation>
#include <QSet>
#include <QRunnable>
#include <QPointer>
#include <QFontMetrics>

class QSGTexture;
class QSGGeometry;

// SafeNodeDeleter definition
class SafeNodeDeleter : public QRunnable {
    public:
        explicit SafeNodeDeleter(QSGNode* node) : m_node(node) {}
        
        void run() override {
            if (m_node) {
                delete m_node;
            }
        }
        
    private:
        QSGNode* m_node;
};

// SafeNodeBatchDeleter definition
class SafeNodeBatchDeleter : public QRunnable {
    public:
        explicit SafeNodeBatchDeleter(const QList<QSGNode*>& nodes) : m_nodes(nodes) {}
        
        void run() override {
            for (QSGNode* node : m_nodes) {
                if (node) {
                    delete node;
                }
            }
            m_nodes.clear();
        }
        
    private:
        QList<QSGNode*> m_nodes;
};

class CustomImageListView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(qreal itemWidth READ itemWidth WRITE setItemWidth NOTIFY itemWidthChanged)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(bool useLocalImages READ useLocalImages WRITE setUseLocalImages NOTIFY useLocalImagesChanged)
    Q_PROPERTY(QString imagePrefix READ imagePrefix WRITE setImagePrefix NOTIFY imagePrefixChanged)
    Q_PROPERTY(QVariantList imageUrls READ imageUrls WRITE setImageUrls NOTIFY imageUrlsChanged)
    Q_PROPERTY(QStringList imageTitles READ imageTitles WRITE setImageTitles NOTIFY imageTitlesChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QVariantList localImageUrls READ localImageUrls WRITE setLocalImageUrls NOTIFY localImageUrlsChanged)
    Q_PROPERTY(QVariantList remoteImageUrls READ remoteImageUrls WRITE setRemoteImageUrls NOTIFY remoteImageUrlsChanged)
    Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount NOTIFY rowCountChanged)
    Q_PROPERTY(qreal contentX READ contentX WRITE setContentX NOTIFY contentXChanged)
    Q_PROPERTY(qreal contentY READ contentY WRITE setContentY NOTIFY contentYChanged)
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight NOTIFY contentHeightChanged)
    Q_PROPERTY(qreal itemHeight READ itemHeight WRITE setItemHeight NOTIFY itemHeightChanged)
    Q_PROPERTY(qreal rowSpacing READ rowSpacing WRITE setRowSpacing NOTIFY rowSpacingChanged)
    Q_PROPERTY(QStringList rowTitles READ rowTitles WRITE setRowTitles NOTIFY rowTitlesChanged)
    Q_PROPERTY(QUrl jsonSource READ jsonSource WRITE setJsonSource NOTIFY jsonSourceChanged)
    Q_PROPERTY(qreal startPositionX READ startPositionX WRITE setStartPositionX NOTIFY startPositionXChanged)
    Q_PROPERTY(int nodeCount READ nodeCount CONSTANT)
    Q_PROPERTY(int textureCount READ textureCount CONSTANT)
    Q_PROPERTY(bool enableNodeMetrics READ enableNodeMetrics WRITE setEnableNodeMetrics NOTIFY enableNodeMetricsChanged)
    Q_PROPERTY(bool enableTextureMetrics READ enableTextureMetrics WRITE setEnableTextureMetrics NOTIFY enableTextureMetricsChanged)

public:
    struct NavigationPosition {
        QString category;
        int relativePosition;
        qreal scrollOffset;
    };

    CustomImageListView(QQuickItem *parent = nullptr);
    ~CustomImageListView();

    int count() const { return m_count; }
    void setCount(int count);

    qreal itemWidth() const { return m_itemWidth; }
    void setItemWidth(qreal width);

    qreal spacing() const { return m_spacing; }
    void setSpacing(qreal spacing);

    bool useLocalImages() const { return m_useLocalImages; }
    void setUseLocalImages(bool local);
    
    QString imagePrefix() const { return m_imagePrefix; }
    void setImagePrefix(const QString &prefix);

    QVariantList imageUrls() const { return m_imageUrls; }
    void setImageUrls(const QVariantList &urls);

    QStringList imageTitles() const { return m_imageTitles; }
    void setImageTitles(const QStringList &titles);

    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);

    QVariantList localImageUrls() const { return m_localImageUrls; }
    void setLocalImageUrls(const QVariantList &urls);
    
    QVariantList remoteImageUrls() const { return m_remoteImageUrls; }
    void setRemoteImageUrls(const QVariantList &urls);
    
    int rowCount() const { return m_rowCount; }
    void setRowCount(int count);

    qreal contentX() const { return m_contentX; }
    void setContentX(qreal x);
    
    qreal contentY() const { return m_contentY; }
    void setContentY(qreal y);
    
    qreal contentWidth() const;
    qreal contentHeight() const;

    qreal itemHeight() const { return m_itemHeight; }
    void setItemHeight(qreal height);
    
    qreal rowSpacing() const { return m_rowSpacing; }
    void setRowSpacing(qreal spacing);
    
    QStringList rowTitles() const { return m_rowTitles; }
    void setRowTitles(const QStringList &titles);

    QUrl jsonSource() const { return m_jsonSource; }
    void setJsonSource(const QUrl &source);

    qreal startPositionX() const { return m_startPositionX; }
    void setStartPositionX(qreal x);

    int nodeCount() const { return m_totalNodeCount; }
    int textureCount() const { return m_textureCount; }
    
    bool enableNodeMetrics() const { return m_enableNodeMetrics; }
    void setEnableNodeMetrics(bool enable);
    
    bool enableTextureMetrics() const { return m_enableTextureMetrics; }
    void setEnableTextureMetrics(bool enable);

    void updateMetricCounts(int nodes, int textures);

signals:
    void countChanged();
    void itemWidthChanged();
    void spacingChanged();
    void useLocalImagesChanged();
    void imagePrefixChanged();
    void imageUrlsChanged();
    void imageTitlesChanged();
    void currentIndexChanged();
    void localImageUrlsChanged();
    void remoteImageUrlsChanged();
    void rowCountChanged();
    void contentXChanged();
    void contentYChanged();
    void contentWidthChanged();
    void contentHeightChanged();
    void itemHeightChanged();
    void rowSpacingChanged();
    void rowTitlesChanged();
    void jsonSourceChanged();
    void linkActivated(const QString& action, const QString& url);
    void startPositionXChanged();
    void moodImageSelected(const QString& url);
    void assetFocused(const QJsonObject& assetData);
    void enableNodeMetricsChanged();
    void enableTextureMetricsChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    // Struct definitions
    struct ImageData {
        QString url;
        QString title;
        QString category;
        QString description;
        QString id;
        QString thumbnailUrl;
        QMap<QString, QString> links;
        
        bool operator==(const ImageData& other) const {
            return url == other.url && 
                   title == other.title && 
                   category == other.category && 
                   id == other.id;
        }
    };

    struct TexturedNode {
        TexturedNode() : node(nullptr), texture(nullptr) {}
        ~TexturedNode() {
            delete node;
            node = nullptr;
            texture = nullptr;
        }
        QSGGeometryNode *node;
        QSGTexture *texture;
    };

    struct CategoryDimensions {
        int rowHeight;
        int posterHeight;
        int posterWidth;
        qreal itemSpacing;
    };

    // Member variables
    QNetworkAccessManager* m_networkManager = nullptr;
    QVector<ImageData> m_imageData;
    qreal m_startPositionX = 0;
    int m_count = 15;
    qreal m_itemWidth = 200;
    qreal m_itemHeight = 200;
    qreal m_spacing = 10;
    qreal m_rowSpacing = 10;
    bool m_useLocalImages = false;
    QString m_imagePrefix = "qrc:/images/";
    QVariantList m_imageUrls;
    QStringList m_imageTitles;
    int m_currentIndex = 0;
    QVariantList m_localImageUrls;
    QVariantList m_remoteImageUrls;
    int m_rowCount = 2;
    qreal m_contentX = 0;
    qreal m_contentY = 0;
    QStringList m_rowTitles;
    bool m_windowReady = false;
    bool m_isDestroying = false;
    bool m_isLoading = false;
    int m_itemsPerRow = 5;
    QUrl m_jsonSource;
    QMutex m_loadMutex;
    bool m_enableNodeMetrics = false;
    bool m_enableTextureMetrics = false;
    int m_titleHeight = 25;
    int m_maxRows = 4;
    QMap<QString, qreal> m_categoryContentX;
    QString m_currentCategory;
    QMap<QString, CategoryDimensions> m_categoryDimensions;
    QPropertyAnimation* m_scrollAnimation;
    QMap<QString, QPropertyAnimation*> m_categoryAnimations;
    QJsonObject m_parsedJson;
    bool m_isBeingDestroyed = false;
    int m_nodeCount = 0;
    int m_totalNodeCount = 0;
    QAtomicInt m_textureCount = 0;
    QMutex m_networkMutex;
    QHash<int, QNetworkReply*> m_pendingRequests;
    QHash<QUrl, QImage> m_urlImageCache;
    QMap<int, TexturedNode> m_nodes;
    QMap<QString, NavigationPosition> m_categoryPositions;

    // Texture management constants
    static const int MAX_TEXTURE_CACHE = 20;  // Max textures to keep in memory
    static const int MAX_VISIBLE_ROWS = 3;    // Maximum number of visible rows to keep in memory
    static const int CLEANUP_THRESHOLD = MAX_VISIBLE_ROWS * 10;  // Cleanup threshold based on visible rows
    
    QQueue<int> m_textureUsageQueue;    // Track texture usage order
    QSet<int> m_visibleIndices;         // Currently visible indices
    QSet<QString> m_visibleCategories;   // Track visible row categories

    // Method declarations (one per method)
    void addDefaultItems();
    void ensureValidIndex(int &index);
    void handleContentPositionChange();
    void animateVerticalScroll(qreal targetY);
    qreal calculateItemVerticalPosition(int index);
    bool navigateLeft();
    void navigateRight();
    void navigateUp();
    void navigateDown();
    bool shouldJumpToLastRow(int currentIndex) const;  // Add this line
    void handleKeyAction(Qt::Key key);
    void ensureFocus();
    void ensureIndexVisible(int index);
    void cleanupNode(TexturedNode& node);
    void limitTextureCacheSize(int maxTextures);
    void cleanupTextures();
    void safeReleaseTextures();
    void safeCleanup();
    void processLoadedImage(int index, const QImage &image);
    void loadImage(int index);
    void loadUrlImage(int index, const QUrl &url);
    void createFallbackTexture(int index);
    QImage loadLocalImageFromPath(const QString &path) const;
    QImage loadLocalImage(int index) const;
    QString generateImageUrl(int index) const;
    bool isReadyForTextures() const;
    void tryLoadImages();
    void loadAllImages();
    void addSelectionEffects(QSGNode* container, const QRectF& rect);
    void addTitleOverlay(QSGNode* container, const QRectF& rect, const QString& title);
    void createTitleNode(const QString &text, const QRectF &rect);
    QSGGeometryNode* createTexturedRect(const QRectF &rect, QSGTexture *texture, bool isFocused = false);
    bool ensureValidWindow() const;
    QSGGeometryNode* createOptimizedTextNode(const QString &text, const QRectF &rect);
    QSGGeometryNode* createRowTitleNode(const QString &text, const QRectF &rect);
    void loadFromJson(const QUrl &source);
    void loadUISettings();
    void processJsonData(const QByteArray &data);
    void debugResourceSystem() const;
    qreal categoryContentWidth(const QString& category) const;
    void setCategoryContentX(const QString& category, qreal x);
    qreal getCategoryContentX(const QString& category) const;
    void updateCurrentCategory();
    void saveNavigationPosition(const QString& category);
    NavigationPosition getLastPosition(const QString& category) const;
    int findMatchingPositionIndex(const QString& category, int relativePosition) const;
    void setupNetworkManager();
    void setupScrollAnimation();
    void animateScroll(const QString& category, qreal targetX);
    void stopCurrentAnimation();
    int countNodes(QSGNode *root);
    void collectTextures(QSGNode *node, QSet<QSGTexture *> &textures);
    int countTotalTextures(QSGNode *root);
    QVector<int> getVisibleIndices();
    QVector<int> getVisibleIndices(qreal bufferFactor = 0.5); // Add overloaded version that takes a buffer factor
    void debugLastRowVisibility(); // Add debug helper

    // Category dimensions helper
    CategoryDimensions getDimensionsForCategory(const QString& category) const;

    // New helper method for processing swimlane dimensions
    void processSwimlaneDimensions(const QJsonObject &swimlaneObj);

    void recycleOffscreenTextures();
    void cacheTexture(int index, QSGTexture* texture);
    bool isIndexVisible(int index) const;

    int getCurrentItemPositionInCategory() const;
    void preloadRowTextures(const QString& category);

    int findMatchingPositionInNextCategory(const QString& currentCategory, const QString& nextCategory);
    qreal calculateRequiredScrollPosition(int targetIndex);

    void ensureLastRowVisible(int targetIndex);
    void animateToPosition(int targetIndex, qreal desiredY);

    // Add this declaration
    void loadVisibleTextures();
    void ensureLastRowFullyVisible();

private slots:
    void onNetworkReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError code);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onScrollAnimationValueChanged(const QVariant &value);
};

#endif // CUSTOMIMAGELISTVIEW_H
