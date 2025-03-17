#ifndef CUSTOMIMAGELISTVIEW_H
#define CUSTOMIMAGELISTVIEW_H
#include <QQmlEngine>
#include <QQuickItem>
#include <QList>
#include <QUrl>
#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QMap>

#include <QImage>
#include <QSGSimpleTextureNode>
#include <QSGTextureMaterial>

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
#include <QSGSimpleTextureNode>
class QSGTexture;
class QSGGeometry;

// SafeNodeDeleter definition
class SafeNodeDeleter : public QRunnable {
    public:
        explicit SafeNodeDeleter(QSGNode* node) : m_node(node) {}

        void run() override {
            if (m_node) {
                delete m_node;
    void debugLastRowVisibility(); // Add debug helper
}

    private:
        QSGNode* m_node;
};
    CategoryDimensions getDimensionsForCategory(const QString& category) const;
// SafeNodeBatchDeleter definition
class SafeNodeBatchDeleter : public QRunnable {
    public:
        explicit SafeNodeBatchDeleter(const QList<QSGNode*>& nodes) : m_nodes(nodes) {}
    void processSwimlaneDimensions(const QJsonObject &swimlaneObj);
        void run() override {
            for (QSGNode* node : m_nodes) {
                if (node) {
                    delete node;


            m_nodes.clear();


    private:
        QList<QSGNode*> m_nodes;
};
                debugLastRowVisibility(); // New debugging helper
class CustomImageListView : public QQuickItem

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
    qreal itemBottom = itemTop + itemHeight;
public:
    struct NavigationPosition {
        QString category;
        int relativePosition;
        qreal scrollOffset;
    };
    qreal viewportTop = m_contentY;
    CustomImageListView(QQuickItem *parent = nullptr);
    ~CustomImageListView();
    qreal viewportBottom = viewportTop + height();
    int count() const { return m_count; }
    void setCount(int count);
    
    qreal itemWidth() const { return m_itemWidth; }
    void setItemWidth(qreal width);
    qDebug() << "Last Row Visibility Check:";
    qreal spacing() const { return m_spacing; }
    void setSpacing(qreal spacing);
    qDebug() << "  Item rectangle: top=" << itemTop << "bottom=" << itemBottom;
    bool useLocalImages() const { return m_useLocalImages; }
    void setUseLocalImages(bool local);

    QString imagePrefix() const { return m_imagePrefix; }
    void setImagePrefix(const QString &prefix);
void CustomImageListView::ensureIndexVisible(int index)
    QVariantList imageUrls() const { return m_imageUrls; }
    void setImageUrls(const QVariantList &urls);
{
    QStringList imageTitles() const { return m_imageTitles; }
    void setImageTitles(const QStringList &titles);
    if (index < 0 || index >= m_imageData.size()) {
    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);
        return;
    QVariantList localImageUrls() const { return m_localImageUrls; }
    void setLocalImageUrls(const QVariantList &urls);

    QVariantList remoteImageUrls() const { return m_remoteImageUrls; }
    void setRemoteImageUrls(const QVariantList &urls);

    int rowCount() const { return m_rowCount; }
    void setRowCount(int count);
    }
    qreal contentX() const { return m_contentX; }
    void setContentX(qreal x);

    qreal contentY() const { return m_contentY; }
    void setContentY(qreal y);

    qreal contentWidth() const;
    qreal contentHeight() const;
            itemCount++;
    qreal itemHeight() const { return m_itemHeight; }
    void setItemHeight(qreal height);

    qreal rowSpacing() const { return m_rowSpacing; }
    void setRowSpacing(qreal spacing);

    QStringList rowTitles() const { return m_rowTitles; }
    void setRowTitles(const QStringList &titles);
            if (isLastCategory) {
    QUrl jsonSource() const { return m_jsonSource; }
    void setJsonSource(const QUrl &source);
                position = qMin(position, contentHeight() - dims.rowHeight - m_titleHeight);
    qreal startPositionX() const { return m_startPositionX; }
    void setStartPositionX(qreal x);
            }
    int nodeCount() const { return m_totalNodeCount; }
    int textureCount() const { return m_textureCount; }

    bool enableNodeMetrics() const { return m_enableNodeMetrics; }
    void setEnableNodeMetrics(bool enable);

    bool enableTextureMetrics() const { return m_enableTextureMetrics; }
    void setEnableTextureMetrics(bool enable);
    if (m_localImageUrls != urls) {
    void updateMetricCounts(int nodes, int textures);
        m_localImageUrls = urls;
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
        emit localImageUrlsChanged();
protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
        loadAllImages();
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

    };
    // Calculate the maximum allowed scroll position
    struct TexturedNode {
        TexturedNode() : node(nullptr), texture(nullptr) {}
        ~TexturedNode() {
            delete node;
            node = nullptr;
            texture = nullptr;

        QSGGeometryNode *node;
        QSGTexture *texture;
    };
    for (int categoryIndex = 0; categoryIndex < m_rowTitles.size(); ++categoryIndex) {
    struct CategoryDimensions {
        int rowHeight;
        int posterHeight;
        int posterWidth;
        qreal itemSpacing;


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
    static const int MAX_VISIBLE_ROWS = 6;    // Increased from 3 to 6
    static const int CLEANUP_THRESHOLD = MAX_VISIBLE_ROWS * 10;  // Cleanup threshold updated
                setRowSpacing(kv3Config["rowSpacing"].toInt());
    QQueue<int> m_textureUsageQueue;    // Track texture usage order
    QSet<int> m_visibleIndices;         // Currently visible indices
    QSet<QString> m_visibleCategories;   // Track visible row categories
                qDebug() << "Setting row spacing from JSON:" << m_rowSpacing;
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
            } else {
    // Category dimensions helper
    CategoryDimensions getDimensionsForCategory(const QString& category) const;
                qDebug() << "No rowSpacing in JSON, using default:" << m_rowSpacing;
    // New helper method for processing swimlane dimensions
    void processSwimlaneDimensions(const QJsonObject &swimlaneObj);
            }
    void recycleOffscreenTextures();
    void cacheTexture(int index, QSGTexture* texture);
    bool isIndexVisible(int index) const;
            
    int getCurrentItemPositionInCategory() const;
    void preloadRowTextures(const QString& category);
            // Get row title configuration
    int findMatchingPositionInNextCategory(const QString& currentCategory, const QString& nextCategory);
    qreal calculateRequiredScrollPosition(int targetIndex);

    void ensureLastRowVisible(int targetIndex);
    void animateToPosition(int targetIndex, qreal desiredY);

    // Add this declaration
    void loadVisibleTextures();
    void ensureLastRowFullyVisible();

    // Helper method to check if right edge is visible
    bool isRightEdgeVisible(const QString& category) const;

    // Add missing method declaration
    void updateVisibleCategories();
                qDebug() << "Setting title height from JSON:" << m_titleHeight;
private slots:
    void onNetworkReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError code);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onScrollAnimationValueChanged(const QVariant &value);
};
            } else {
#endif // CUSTOMIMAGELISTVIEW_H

        QTimer::singleShot(50, this, [this]() {

            // Ensure we're at the very bottom

            setContentY(contentHeight() - height());

            update();

        });

    });

    

    connect(anim, &QPropertyAnimation::finished, anim, &QPropertyAnimation::deleteLater);

    anim->start();

    

    // Preload textures for last row

    QString category = m_imageData[targetIndex].category;

    m_visibleCategories.insert(category);

    preloadRowTextures(category);

}



// Add this helper method for animation

void CustomImageListView::animateToPosition(int targetIndex, qreal desiredY)

{

    QPropertyAnimation* anim = new QPropertyAnimation(this, "contentY");

    anim->setDuration(300);

    anim->setEasingCurve(QEasingCurve::OutCubic);

    anim->setStartValue(m_contentY);

    anim->setEndValue(qMax(0.0, desiredY));

    

    connect(anim, &QPropertyAnimation::finished, this, [this, targetIndex]() {

        setCurrentIndex(targetIndex);

        ensureFocus();

        update();

    });

    

    connect(anim, &QPropertyAnimation::finished, anim, &QPropertyAnimation::deleteLater);

    anim->start();

    

    // Preload textures

    QString category = m_imageData[targetIndex].category;

    m_visibleCategories.insert(category);

    QTimer::singleShot(0, this, [this, category]() {

        preloadRowTextures(category);

    });

}



// Add new helper method to force immediate texture loading for visible items

void CustomImageListView::loadVisibleTextures()

{

    // Use a wider buffer to improve loading when navigating

    QVector<int> visibleIndices = getVisibleIndices(0.8); // 80% buffer (larger than default)

    

    for (int index : visibleIndices) {

        if (!m_nodes.contains(index)) {

            // Use a shorter delay to ensure quick loading

            QTimer::singleShot(5, this, [this, index]() {

                if (!m_isBeingDestroyed) {

                    loadImage(index);

                }

            });

        }

    }

    

    // Also preload for current category and neighboring categories

    if (m_currentIndex >= 0 && m_currentIndex < m_imageData.size()) {

        QString currentCategory = m_imageData[m_currentIndex].category;

        int categoryIndex = m_rowTitles.indexOf(currentCategory);

        

        // Preload current category

        preloadRowTextures(currentCategory);

        

        // Preload category above (if exists)

        if (categoryIndex > 0) {

            preloadRowTextures(m_rowTitles[categoryIndex - 1]);

        }

        

        // Preload category below (if exists)

        if (categoryIndex < m_rowTitles.size() - 1) {

            preloadRowTextures(m_rowTitles[categoryIndex + 1]);

        }

    }

}



// Add this overloaded method for more flexibility in buffer size

QVector<int> CustomImageListView::getVisibleIndices(qreal bufferFactor)

{

    QVector<int> visibleIndices;

    

    if (width() <= 0 || height() <= 0) {

        return visibleIndices;

    }

    

    // Calculate buffer size based on provided factor

    qreal bufferSize = height() * bufferFactor; 

    qreal viewportTop = m_contentY - bufferSize;

    qreal viewportBottom = m_contentY + height() + bufferSize;

    

    qreal currentY = 0;

    int currentIndex = 0;

    

    // Iterate through categories

    for (int categoryIndex = 0; categoryIndex < m_rowTitles.size(); ++categoryIndex) {

        QString categoryName = m_rowTitles[categoryIndex];

        CategoryDimensions dims = getDimensionsForCategory(categoryName);

        

        // Add title height

        currentY += m_titleHeight + 10;

        

        // Skip if entire category is below viewport

        if (currentY > viewportBottom) {

            // Skip to next category after counting items in this one

            for (const ImageData &imgData : m_imageData) {

                if (imgData.category == categoryName) {

                    currentIndex++;

                }

            }

            // Add category height and continue

            currentY += dims.rowHeight + m_rowSpacing;

            continue;

        }

        

        // Calculate category end position

        qreal categoryEndY = currentY + dims.rowHeight;

        

        // Skip if entire category is above viewport

        if (categoryEndY < viewportTop) {

            // Skip this category

            for (const ImageData &imgData : m_imageData) {

                if (imgData.category == categoryName) {

                    currentIndex++;

                }

            }

            currentY = categoryEndY + m_rowSpacing;

            continue;

        }

        

        // Use a wider horizontal buffer for categories

        qreal horizontalBuffer = width() * 0.5; // 50% of width as buffer

        qreal xPos = m_startPositionX + 10;

        qreal categoryX = xPos - getCategoryContentX(categoryName);

        int itemsInThisCategory = 0;

        

        for (int i = 0; i < m_imageData.size(); i++) {

            if (m_imageData[i].category == categoryName) {

                // Use wider horizontal buffer for visibility check

                if (currentY >= viewportTop - dims.posterHeight && 

                    currentY <= viewportBottom + dims.posterHeight &&

                    categoryX >= -dims.posterWidth - horizontalBuffer &&

                    categoryX <= width() + dims.posterWidth + horizontalBuffer) {

                    

                    visibleIndices.append(currentIndex);

                }

                

                categoryX += dims.posterWidth + dims.itemSpacing;

                currentIndex++;

                itemsInThisCategory++;

            }

        }

        

        // Move to next category

        currentY = categoryEndY + m_rowSpacing;

    }

    

    return visibleIndices;

}

