#ifndef CUSTOMIMAGELISTVIEW_H
#define CUSTOMIMAGELISTVIEW_H

#include <QQuickItem>
#include <QList>
#include <QUrl>
#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QMap>
#include <QSGGeometry>
#include <QImage>
#include <QSGSimpleTextureNode>

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QQueue>  // Add this include
#include "texturebuffer.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSGOpaqueTextureMaterial>

#include <QNetworkConfiguration>
#include <QSslError>
#include <QPropertyAnimation>
#include <QSet>
#include <QRunnable>
#include <cmath>
#include <QFontMetrics>
#include <QtMath>
class QSGTexture;
class QSGGeometry;

// SafeNodeDeleter definition
class SafeNodeDeleter : public QRunnable {
    public:
        explicit SafeNodeDeleter(QSGNode* node) : m_node(node) {}
    setFocus(true);
        void run() override {
            if (m_node) {
                delete m_node;
    });

    
    private:
        QSGNode* m_node;
};

// SafeNodeBatchDeleter definition
class SafeNodeBatchDeleter : public QRunnable {
    public:
        explicit SafeNodeBatchDeleter(const QList<QSGNode*>& nodes) : m_nodes(nodes) {}
        }
        void run() override {
            for (QSGNode* node : m_nodes) {
                if (node) {
                    delete node;
        

            m_nodes.clear();
void CustomImageListView::setSpacing(qreal spacing)

    private:
        QList<QSGNode*> m_nodes;
};
    if (m_useLocalImages != local) {
class CustomImageListView : public QQuickItem
        loadAllImages();
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
            .arg(index + 1);
    CustomImageListView(QQuickItem *parent = nullptr);
    ~CustomImageListView();

    int count() const { return m_count; }
    void setCount(int count);
    int actualIndex = (index % 5) + 1; // We have img1.jpg through img5.jpg
    qreal itemWidth() const { return m_itemWidth; }
    void setItemWidth(qreal width);
    imagePath = QString(":/data/images/img%1.jpg").arg(actualIndex);
    qreal spacing() const { return m_spacing; }
    void setSpacing(qreal spacing);
        QImage image;
    bool useLocalImages() const { return m_useLocalImages; }
    void setUseLocalImages(bool local);

    QString imagePrefix() const { return m_imagePrefix; }
    void setImagePrefix(const QString &prefix);

    QVariantList imageUrls() const { return m_imageUrls; }
    void setImageUrls(const QVariantList &urls);

    QStringList imageTitles() const { return m_imageTitles; }
    void setImageTitles(const QStringList &titles);
    painter.setPen(Qt::white);
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
    if (m_isBeingDestroyed || !ensureValidWindow() || index >= m_imageData.size()) {
    qreal contentY() const { return m_contentY; }
    void setContentY(qreal y);

    qreal contentWidth() const;
    qreal contentHeight() const;
        });
    qreal itemHeight() const { return m_itemHeight; }
    void setItemHeight(qreal height);

    qreal rowSpacing() const { return m_rowSpacing; }
    void setRowSpacing(qreal spacing);
    if (m_nodes.contains(index) && m_nodes[index].texture) {
    QStringList rowTitles() const { return m_rowTitles; }
    void setRowTitles(const QStringList &titles);
    // If not a local resource, try as network URL
    QUrl jsonSource() const { return m_jsonSource; }
    void setJsonSource(const QUrl &source);
        if (imagePath.startsWith("//")) {
    qreal startPositionX() const { return m_startPositionX; }
    void setStartPositionX(qreal x);
    } else {
    int nodeCount() const { return m_totalNodeCount; }
    int textureCount() const { return m_textureCount; }

    bool enableNodeMetrics() const { return m_enableNodeMetrics; }
    void setEnableNodeMetrics(bool enable);
{
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
            node.texture = texture;
protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    qDebug() << "Failed to load image from:" << path;
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
            QMutexLocker locker(&m_networkMutex);
    struct TexturedNode {
        TexturedNode() : node(nullptr), texture(nullptr) {}
        ~TexturedNode() {
            delete node;
            node = nullptr;
            texture = nullptr;
        createFallbackTexture(index);
        QSGGeometryNode *node;
        QSGTexture *texture;
    };
    }
    struct CategoryDimensions {
        int rowHeight;
        int posterHeight;
        int posterWidth;
        qreal itemSpacing;
            qDebug() << " - File does not exist";

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
    }
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
}
    // Category dimensions helper
    CategoryDimensions getDimensionsForCategory(const QString& category) const;
        }
    // New helper method for processing swimlane dimensions
    void processSwimlaneDimensions(const QJsonObject &swimlaneObj);
{
    void recycleOffscreenTextures();
    void cacheTexture(int index, QSGTexture* texture);
    bool isIndexVisible(int index) const;
            event->accept();
    int getCurrentItemPositionInCategory() const;
    void preloadRowTextures(const QString& category);

    int findMatchingPositionInNextCategory(const QString& currentCategory, const QString& nextCategory);
    qreal calculateRequiredScrollPosition(int targetIndex);
            event->accept();
    void ensureLastRowVisible(int targetIndex);
    void animateToPosition(int targetIndex, qreal desiredY);

    // Add this declaration
    void loadVisibleTextures();
    void ensureLastRowFullyVisible();
        case Qt::Key_Down:
private slots:
    void onNetworkReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError code);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onScrollAnimationValueChanged(const QVariant &value);
};

#endif // CUSTOMIMAGELISTVIEW_H
    }



    // Get dimensions for focused item

    CategoryDimensions dims = getDimensionsForCategory(category);

    

    // Calculate actual vertical position of the item

    qreal itemTop = calculateItemVerticalPosition(m_currentIndex);

    qreal itemHeight = dims.posterHeight;

    qreal focusPadding = itemHeight * 0.25; // 25% extra for focus effects

    

    // Calculate item bounds including focus effects

    qreal itemBottom = itemTop + itemHeight + focusPadding;

    

    // Get viewport bounds

    qreal viewportTop = m_contentY;

    qreal viewportBottom = viewportTop + height();

    

    qDebug() << "Last row visibility check:";

    qDebug() << "  Item top:" << itemTop << "Item bottom:" << itemBottom;

    qDebug() << "  Viewport top:" << viewportTop << "Viewport bottom:" << viewportBottom;

    

    // Check if item is fully visible with focus effects

    bool fullyVisible = (itemTop >= viewportTop && itemBottom <= viewportBottom);

    if (!fullyVisible) {

        qDebug() << "Last row not fully visible, adjusting";

        

        // If item is too high (top above viewport) or too low (bottom below viewport)

        qreal newY;

        

        // Always prioritize showing the bottom with focus effects

        if (itemBottom > viewportBottom) {

            // Adjust so the bottom with focus effects is visible at 95% of viewport height

            newY = itemBottom - (height() * 0.95);

        } else if (itemTop < viewportTop) {

            // If top is not visible, show the item from the top

            newY = itemTop;

        } else {

            // Should not happen, but just in case

            return;

        }

        

        // Apply the new position with a short animation for smoother experience

        QPropertyAnimation* anim = new QPropertyAnimation(this, "contentY");

        anim->setDuration(200);

        anim->setEasingCurve(QEasingCurve::OutQuad);

        anim->setStartValue(m_contentY);

        anim->setEndValue(newY);

        

        connect(anim, &QPropertyAnimation::finished, anim, &QPropertyAnimation::deleteLater);

        connect(anim, &QPropertyAnimation::finished, this, [this]() {

            update();

            // Double-check after animation to ensure we really got it right

            QTimer::singleShot(50, this, [this]() {

                debugLastRowVisibility(); // New debugging helper

            });

        });

        

        anim->start();

    } else {

        qDebug() << "Last row is fully visible already";

    }

}



// Add this debugging helper

void CustomImageListView::debugLastRowVisibility()

{

    if (m_currentIndex < 0 || m_currentIndex >= m_imageData.size()) {

        return;

    }

    

    QString category = m_imageData[m_currentIndex].category;

    int categoryIndex = m_rowTitles.indexOf(category);

    

    if (categoryIndex != m_rowTitles.size() - 1) {

        return; // Not the last row

    }

    

    CategoryDimensions dims = getDimensionsForCategory(category);

    qreal itemTop = calculateItemVerticalPosition(m_currentIndex);

    qreal itemHeight = dims.posterHeight;

    qreal itemBottom = itemTop + itemHeight;

    qreal viewportTop = m_contentY;

    qreal viewportBottom = viewportTop + height();

    

    qDebug() << "Last Row Visibility Check:";

    qDebug() << "  Item rectangle: top=" << itemTop << "bottom=" << itemBottom;

    qDebug() << "  Viewport: top=" << viewportTop << "bottom=" << viewportBottom;

    qDebug() << "  Top visible:" << (itemTop >= viewportTop);

    qDebug() << "  Bottom visible:" << (itemBottom <= viewportBottom);

}



void CustomImageListView::ensureIndexVisible(int index)

{

    if (index < 0 || index >= m_imageData.size()) {

        return;

    }



    QString category = m_imageData[index].category;

    CategoryDimensions dims = getDimensionsForCategory(category);

    bool isLastCategory = (m_rowTitles.indexOf(category) == m_rowTitles.size() - 1);

    

    // Calculate target position

    qreal targetY = calculateItemVerticalPosition(index);

    qreal rowHeight = dims.rowHeight + m_titleHeight;

    qreal viewportHeight = height();

    

    // Calculate scroll position

    qreal desiredY;

    

    if (isLastCategory) {

        // For last category, always scroll to show the entire row

        desiredY = contentHeight() - viewportHeight;

        

        // Add small padding for visual comfort

        desiredY = qMin(desiredY + (dims.posterHeight * 0.1), contentHeight() - viewportHeight);

    } else {

        // For other categories, center in viewport

        desiredY = targetY - (viewportHeight - rowHeight) / 2;

    }

    

    // Apply bounds

    qreal maxScroll = contentHeight() - viewportHeight;

    desiredY = qBound(0.0, desiredY, maxScroll);

    

    // Set final position

    setContentY(desiredY);



    // Handle horizontal scrolling

    if (category == m_currentCategory) {

        int itemsBeforeInCategory = 0;

        for (int i = 0; i < index; i++) {

            if (m_imageData[i].category == category) {

                itemsBeforeInCategory++;

            }

        }

        

        qreal targetX = itemsBeforeInCategory * (dims.posterWidth + dims.itemSpacing);

        targetX = qMax(0.0, targetX - (width() - dims.posterWidth) / 2);

        

        // Animate the horizontal scroll

        animateScroll(category, targetX);

    }

}



// Add these new helper methods to the class implementation:



int CustomImageListView::findMatchingPositionInNextCategory(const QString& /*currentCategory*/, const QString& nextCategory)

{

    int currentItemPosition = getCurrentItemPositionInCategory();

    int targetIndex = -1;

    int itemCount = 0;

    

    for (int i = 0; i < m_imageData.size(); i++) {

        if (m_imageData[i].category == nextCategory) {

            if (itemCount == currentItemPosition) {

                targetIndex = i;

                break;

            }

            itemCount++;

        }

    }

    

    // If no exact match found, take first item in next category

    if (targetIndex == -1) {

        for (int i = 0; i < m_imageData.size(); i++) {

            if (m_imageData[i].category == nextCategory) {

                targetIndex = i;

                break;

            }

        }

    }

    

    return targetIndex;

}



void CustomImageListView::preloadRowTextures(const QString& category)

{

    int currentPosition = getCurrentItemPositionInCategory();

    int preloadCount = 5;  // Number of items to preload in each direction

    

    // Find visible range in the category

    int visibleStart = qMax(0, currentPosition - preloadCount);

    int itemCount = 0;

    

    for (int i = 0; i < m_imageData.size(); i++) {

        if (m_imageData[i].category == category) {

            if (itemCount >= visibleStart && itemCount <= currentPosition + preloadCount) {

                if (!m_nodes.contains(i)) {

                    loadImage(i);

                }

            }

            itemCount++;

        }

    }

    

    // Clean up old textures if needed

    if (m_nodes.size() > CLEANUP_THRESHOLD) {

        recycleOffscreenTextures();

    }

}



// Add this helper method to calculate item's vertical position

qreal CustomImageListView::calculateItemVerticalPosition(int index)

{

    if (index < 0 || index >= m_imageData.size()) {

        return 0;

    }



    qreal position = 0;

    QString targetCategory = m_imageData[index].category;

    bool isLastCategory = (m_rowTitles.indexOf(targetCategory) == m_rowTitles.size() - 1);

    

    for (const QString &category : m_rowTitles) {

        CategoryDimensions dims = getDimensionsForCategory(category);

        

        // Add spacing between categories

        if (position > 0) {

            position += m_rowSpacing;

        }



        // Add title height

        position += m_titleHeight;



        if (category == targetCategory) {

            // For last category, ensure the entire row height is considered

            if (isLastCategory) {

                position = qMin(position, contentHeight() - dims.rowHeight - m_titleHeight);

            }

            break;

        }



        // Add row height

        position += dims.rowHeight;

    }



    return position;

}





// Add helper method to calculate category width

qreal CustomImageListView::categoryContentWidth(const QString& category) const

{

    // Get the correct dimensions for this category

    CategoryDimensions dims = getDimensionsForCategory(category);

    

    // Count items in this category

    int itemCount = 0;

    for (const ImageData &imgData : m_imageData) {

        if (imgData.category == category) {

            itemCount++;

        }

    }

    

    // Calculate total width using category-specific dimensions

    qreal totalWidth = itemCount * dims.posterWidth + 

                       (itemCount - 1) * dims.itemSpacing;

                       

    // Only add padding if the content is wider than the view

    if (totalWidth > width()) {

        totalWidth += dims.posterWidth * 0.5; // Add half poster width as padding

    }

    

    return totalWidth;

}



bool CustomImageListView::event(QEvent *e)

{

    if (e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut) {

        update();

    }

    if (e->type() == QEvent::DeferredDelete && !m_isBeingDestroyed) {

        m_isBeingDestroyed = true;

        safeCleanup();

    }

    return QQuickItem::event(e);

}



void CustomImageListView::mousePressEvent(QMouseEvent *event)

{

    ensureFocus();

    QQuickItem::mousePressEvent(event);

}



void CustomImageListView::ensureFocus()

{

    if (!hasActiveFocus()) {

        setFocus(true);

        forceActiveFocus();

    }

}



void CustomImageListView::setLocalImageUrls(const QVariantList &urls)

{

    if (m_localImageUrls != urls) {

        m_localImageUrls = urls;

        emit localImageUrlsChanged();

        loadAllImages();

    }

}



void CustomImageListView::setRemoteImageUrls(const QVariantList &urls)

{

    if (m_remoteImageUrls != urls) {

        m_remoteImageUrls = urls;

        emit remoteImageUrlsChanged();

        loadAllImages();

    }

}



void CustomImageListView::setRowCount(int count)

{

    if (m_rowCount != count) {

        m_rowCount = count;

        emit rowCountChanged();

        update();

    }

}



void CustomImageListView::setContentX(qreal x)

{

    if (m_contentX != x) {

        m_contentX = x;

        emit contentXChanged();

        update();

    }

}



void CustomImageListView::setContentY(qreal y)

{

    // Calculate the maximum allowed scroll position

    qreal maxScroll = contentHeight() - height();

    

    // Ensure we don't scroll past the bottom plus padding

    qreal paddedY = qMin(y, maxScroll + 50); // Add extra padding for focus effects

    

    if (m_contentY != paddedY) {

        m_contentY = paddedY;

        emit contentYChanged();

        handleContentPositionChange();

        update();

    }

}



qreal CustomImageListView::contentWidth() const

{

    return m_itemsPerRow * m_itemWidth + (m_itemsPerRow - 1) * m_spacing;

}



// Modified content height with extra padding for last row

qreal CustomImageListView::contentHeight() const

{

    qreal totalHeight = 0;

    

    // Initial padding

    totalHeight += m_rowSpacing / 5;

    

    for (int categoryIndex = 0; categoryIndex < m_rowTitles.size(); ++categoryIndex) {

        // Add spacing between categories

        if (categoryIndex > 0) {

            totalHeight += m_rowSpacing * 0.3;

        }



        totalHeight += m_titleHeight;



        QString categoryName = m_rowTitles[categoryIndex];

        CategoryDimensions dims = getDimensionsForCategory(categoryName);

        int categoryImageCount = 0;

        

        for (const ImageData &imgData : m_imageData) {

            if (imgData.category == categoryName) {

                categoryImageCount++;

            }

        }



        // Add height for each category's contents

        totalHeight += dims.posterHeight;

        

        // Add extra padding for last category to ensure focus effects are visible

        if (categoryIndex == m_rowTitles.size() - 1) {

            // Add 50% of poster height as extra padding for the last row

            totalHeight += dims.posterHeight * 0.5;

        }

    }



    // Add bottom padding plus extra space to ensure last row can be scrolled into view

    totalHeight += m_rowSpacing + 50;

    

    return totalHeight;

}



// Update wheelEvent to handle per-category scrolling

void CustomImageListView::wheelEvent(QWheelEvent *event)

{

    if (event->modifiers() & Qt::ShiftModifier) {

        // Horizontal scrolling within current category

        QString currentCategory = m_imageData[m_currentIndex].category;

        qreal horizontalDelta = event->angleDelta().x() / 120.0 * m_itemWidth;

        qreal newX = m_contentX - horizontalDelta;

        newX = qBound(0.0, newX, qMax(0.0, categoryContentWidth(currentCategory) - width()));

        setCategoryContentX(currentCategory, newX);

    } else {

        // Vertical scrolling between categories

        qreal verticalDelta = event->angleDelta().y() / 120.0 * m_itemHeight;

        qreal newY = m_contentY - verticalDelta;

        newY = qBound(0.0, newY, qMax(0.0, contentHeight() - height()));

        setContentY(newY);

    }

    

    event->accept();

}



void CustomImageListView::cleanupNode(TexturedNode& node)

{

    if (node.node) {

        delete node.node;

        node.node = nullptr;

    }

    // Don't delete texture, just clear the pointer

    node.texture = nullptr;

}



void CustomImageListView::limitTextureCacheSize(int maxTextures)

{

    if (m_nodes.size() > maxTextures) {

        // Remove oldest textures

        auto it = m_nodes.begin();

        while (m_nodes.size() > maxTextures && it != m_nodes.end()) {

            cleanupNode(it.value());

            it = m_nodes.erase(it);

        }

    }

}



// Update cleanupTextures

void CustomImageListView::cleanupTextures()

{

    for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it) {

        cleanupNode(it.value());

    }

    m_nodes.clear();

}



void CustomImageListView::setJsonSource(const QUrl &source)

{

    if (m_jsonSource != source) {

        m_jsonSource = source;

        loadFromJson(source);

        emit jsonSourceChanged();

    }

}



void CustomImageListView::loadFromJson(const QUrl &source)

{

    qDebug() << "Loading JSON from source:" << source.toString();

    

    // First load UI settings

    loadUISettings();

    

    // Then load menu data

    QString menuPath;

    if (source.scheme() == "qrc") {

        menuPath = ":" + source.path();

        qDebug() << "Converted QRC path to:" << menuPath;

    } else {

        menuPath = source.toLocalFile();

        qDebug() << "Using local file path:" << menuPath;

    }

    

    QFile menuFile(menuPath);

    qDebug() << "Attempting to open file:" << menuPath;

    qDebug() << "File exists:" << menuFile.exists();

    

    if (!menuFile.open(QIODevice::ReadOnly)) {

        qWarning() << "Failed to load menu data from:" << menuPath 

                   << "Error:" << menuFile.errorString();

        

        // Try alternative path

        QString altPath = ":/data/embeddedHubMenu.json";

        QFile altFile(altPath);

        qDebug() << "Trying alternative path:" << altPath 

                 << "Exists:" << altFile.exists();

        

        if (altFile.open(QIODevice::ReadOnly)) {

            QByteArray data = altFile.readAll();

            qDebug() << "Successfully read alternative file, size:" << data.size();

            processJsonData(data);

            altFile.close();

        } else {

            qWarning() << "Failed to load menu data from alternative path:" << altPath 

                      << "Error:" << altFile.errorString();

        }

    } else {

        QByteArray data = menuFile.readAll();

        qDebug() << "Successfully read file, size:" << data.size();

        menuFile.close();

        processJsonData(data);

    }

}



// Update loadUISettings method to add more debugging for transparency

void CustomImageListView::loadUISettings()

{

    QString settingsPath = ":/data/uiSettings.json";

    QFile settingsFile(settingsPath);

    

    qDebug() << "Loading UI settings from:" << settingsPath;

    

    if (settingsFile.open(QIODevice::ReadOnly)) {

        QByteArray jsonData = settingsFile.readAll();

        settingsFile.close();

        

        QJsonParseError parseError;

        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        

        if (parseError.error != QJsonParseError::NoError) {

            qWarning() << "JSON parse error:" << parseError.errorString();

            return;

        }

        

        if (doc.isObject()) {

            QJsonObject root = doc.object();

            QJsonObject uiConfig = root["uiConfigurations"].toObject();

            QJsonObject stbConfig = uiConfig["STB"].toObject();

            QJsonObject kv3Config = stbConfig["kv3"].toObject();

            

            // Get row spacing from configuration

            if (kv3Config.contains("rowSpacing")) {

                setRowSpacing(kv3Config["rowSpacing"].toInt());

                qDebug() << "Setting row spacing from JSON:" << m_rowSpacing;

            } else {

                qDebug() << "No rowSpacing in JSON, using default:" << m_rowSpacing;

            }

            

            // Get row title configuration

            QJsonObject rowTitleConfig = kv3Config["rowTitle"].toObject();

            if (rowTitleConfig.contains("Height")) {

                m_titleHeight = rowTitleConfig["Height"].toInt();

                qDebug() << "Setting title height from JSON:" << m_titleHeight;

            } else {

                qDebug() << "No title height in JSON, using default:" << m_titleHeight;

            }

            

            // Get swimlane configuration - process in order of priority

            QJsonObject swimlaneConfig = kv3Config["swimlaneSizeConfiguration"].toObject();

            

            // Process poster config first (these are base values)

            if (swimlaneConfig.contains("poster")) {

                qDebug() << "Processing 'poster' configuration";

                processSwimlaneDimensions(swimlaneConfig["poster"].toObject());

            }

            

            // Process posterWithMetaData config second (these take precedence)

            if (swimlaneConfig.contains("posterWithMetaData")) {

                qDebug() << "Processing 'posterWithMetaData' configuration";

                processSwimlaneDimensions(swimlaneConfig["posterWithMetaData"].toObject());

            }

            

            // Final dimensions summary

            qDebug() << "Final UI dimensions:";

            qDebug() << " - Row spacing:" << m_rowSpacing;

            qDebug() << " - Title height:" << m_titleHeight;

            qDebug() << " - Default item dimensions - Width:" << m_itemWidth 

                     << "Height:" << m_itemHeight << "Spacing:" << m_spacing;

            

            // Dump all category dimensions for debugging

            qDebug() << "Category dimensions:";

            for (auto it = m_categoryDimensions.constBegin(); it != m_categoryDimensions.constEnd(); ++it) {

                qDebug() << " - " << it.key() << ":"

                         << "posterWidth=" << it.value().posterWidth

                         << "posterHeight=" << it.value().posterHeight

                         << "rowHeight=" << it.value().rowHeight

                         << "spacing=" << it.value().itemSpacing;

            }

        } else {

            qWarning() << "JSON document is not an object";

        }

    } else {

        qWarning() << "Failed to load UI settings file:" << settingsFile.errorString();

        

        // Default fallback values with reduced spacing

        setRowSpacing(6);  // Further reduced from 9 to 6

        setItemWidth(240);  // From landscapeType2

        setItemHeight(135); // From landscapeType2

        setSpacing(24);     // From landscapeType2

        m_titleHeight = 15;  // Further reduced from 20 to 15

        

        // Create default dimensions using values from the spec

        CategoryDimensions landscapeDims;

        landscapeDims.rowHeight = 131;  // Further reduced from 133 to 131

        landscapeDims.posterHeight = 135;

        landscapeDims.posterWidth = 240;

        landscapeDims.itemSpacing = 24;

        

        CategoryDimensions portraitDims;

        portraitDims.rowHeight = 224;  // Further reduced from 226 to 224

        portraitDims.posterHeight = 228;

        portraitDims.posterWidth = 152;

        portraitDims.itemSpacing = 24;

        

        // Apply default dimensions to categories

        m_categoryDimensions["TV Channels"] = landscapeDims;

        m_categoryDimensions["Live TV"] = landscapeDims;

        m_categoryDimensions["TV Shows"] = portraitDims;

        m_categoryDimensions["Movies"] = portraitDims;

        m_categoryDimensions["Bein Series"] = portraitDims;

        

        qDebug() << "Using default hardcoded dimensions based on spec";

    }

}



// Update processSwimlaneDimensions method to directly use the exact values from the JSON

void CustomImageListView::processSwimlaneDimensions(const QJsonObject &swimlaneObj)

{

    if (swimlaneObj.isEmpty()) {

        return;

    }

    

    qDebug() << "Processing swimlane dimensions from JSON...";

    

    // Map the JSON type names to actual content category names

    QMap<QString, QString> categoryMapping = {

        {"default", "Default"},

        {"landscapeType1", "TV Channels"},

        {"landscapeType2", "Live Channels"},

        {"portraitType1", "Movies"},

        {"heroBanner", "Featured"},

        {"settings", "Settings"},

        {"landScapeTypeChannelInfo", "On Demand"}

    };
    

    // Process each type defined in the JSON

    for (auto it = swimlaneObj.constBegin(); it != swimlaneObj.constEnd(); ++it) {

        QString type = it.key();

        QJsonObject typeConfig = it.value().toObject();

        

        if (typeConfig.isEmpty()) {

            qDebug() << "Empty config for type:" << type;

            continue;

        }

        

        // Use the exact values from JSON without modifications

        CategoryDimensions dims;

        dims.rowHeight = typeConfig["height"].toInt();

        dims.posterHeight = typeConfig["posterHeight"].toInt();

        dims.posterWidth = typeConfig["posterWidth"].toInt();

        dims.itemSpacing = typeConfig["itemSpacing"].toDouble();

        

        // Map the JSON type to a category name

        QString categoryName = categoryMapping.value(type, type);

        m_categoryDimensions[categoryName] = dims;

        

        qDebug() << "Set dimensions for" << categoryName << "from" << type << ":"

                 << "height=" << dims.rowHeight

                 << "posterHeight=" << dims.posterHeight

                 << "posterWidth=" << dims.posterWidth

                 << "spacing=" << dims.itemSpacing;

        

        // For the default type, set the global item dimensions

        if (type == "default") {

            setItemWidth(dims.posterWidth);

            setItemHeight(dims.posterHeight);

            setSpacing(dims.itemSpacing);

            

            qDebug() << "Set default item dimensions - Width:" << dims.posterWidth

                     << "Height:" << dims.posterHeight

                     << "Spacing:" << dims.itemSpacing;

        }

        

        // Apply specific dimensions for other content types using exact values

        // from the JSON - This ensures we respect the designer's intentions

        if (type == "portraitType1") {

            m_categoryDimensions["Bein Series"] = dims;

            m_categoryDimensions["TV Shows"] = dims;

        }

        else if (type == "landscapeType2") {

            m_categoryDimensions["TV Channels"] = dims;

            m_categoryDimensions["Live TV"] = dims;

        }

    }

}


void CustomImageListView::processJsonData(const QByteArray &data)

{

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject()) {

        qWarning() << "Invalid JSON data - not an object";

        return;

    }


    m_parsedJson = doc.object();  // Store the complete JSON object

    
    // Get menuItems object first

    QJsonObject menuItems = m_parsedJson["menuItems"].toObject();

    if (menuItems.isEmpty()) {

        qWarning() << "menuItems object is empty or invalid";

        return;

    }


    // Clear existing data

    m_imageData.clear();

    m_rowTitles.clear();


    // Process the outer items array (rows)

    QJsonArray rows = menuItems["items"].toArray();

    for (const QJsonValue &rowVal : rows) {

        QJsonObject row = rowVal.toObject();

        

        // Get row information

        QString classificationId = row["classificationId"].toString();

        QString rowTitle = row["title"].toString();

        

        // Add row title

        m_rowTitles.append(rowTitle);

        

        // Process items in this row

        QJsonArray items = row["items"].toArray();

        for (const QJsonValue &itemVal : items) {

            if (!itemVal.isObject()) continue;

            

            QJsonObject item = itemVal.toObject();

            

            // Skip "viewAll" type items

            if (item["assetType"].toString() == "viewAll") {

                continue;

            }

            

            ImageData imgData;

            imgData.category = rowTitle;  // Use row title as category

            imgData.title = item["title"].toString();

            

            // Get image URL - prioritize moodImageUri then thumbnailUri

            imgData.url = item["moodImageUri"].toString();

            if (imgData.url.isEmpty()) {

                imgData.url = item["thumbnailUri"].toString();

            }

            

            // Add additional metadata

            imgData.id = item["assetType"].toString();

            imgData.description = item["shortSynopsis"].toString();

            

            // Clean up URL if needed

            if (imgData.url.startsWith("//")) {

                imgData.url = "https:" + imgData.url;

            }

            

            // Use default image if no URL

            if (imgData.url.isEmpty()) {

                int index = m_imageData.size() % 5 + 1;

                imgData.url = QString(":/data/images/img%1.jpg").arg(index);

            }

            

            // Process links array

            QJsonArray links = item["links"].toArray();

            for (const QJsonValue &linkVal : links) {

                if (!linkVal.isObject()) continue;

                

                QJsonObject link = linkVal.toObject();

                QString href = link["href"].toString();

                

                // Check for events array first

                QJsonArray events = link["events"].toArray();

                if (!events.isEmpty()) {

                    for (const QJsonValue &event : events) {

                        QString eventType = event.toString().toUpper();

                        imgData.links[eventType] = href;

                    }

                }

                // Check for single event

                else if (link.contains("event")) {

                    QString eventType = link["event"].toString().toUpper();

                    imgData.links[eventType] = href;

                }

            }

            

            qDebug() << "Parsed links for" << imgData.title << ":" << imgData.links;

            m_imageData.append(imgData);

        }

    }


    // Update view

    m_count = m_imageData.size();

    if (m_count > 0) {

        safeReleaseTextures();

        loadAllImages();

        emit countChanged();

        emit rowTitlesChanged();

        update();

    } else {

        qWarning() << "No menu items were loaded!";

        addDefaultItems();

    }

}


void CustomImageListView::addDefaultItems()

{

    qDebug() << "Adding default test items";

    

    m_rowTitles.clear();

    m_rowTitles.append("Test Items");

    

    for (int i = 0; i < 5; i++) {

        ImageData imgData;

        imgData.category = "Test Items";

        imgData.title = QString("Test Item %1").arg(i + 1);

        imgData.url = QString(":/data/images/img%1.jpg").arg(i % 5 + 1);

        imgData.id = QString::number(i);

        m_imageData.append(imgData);

    }

    

    m_count = m_imageData.size();

    safeReleaseTextures();

    loadAllImages();

    emit countChanged();

    emit rowTitlesChanged();

    update();

}



// Add helper methods for per-category scrolling

void CustomImageListView::setCategoryContentX(const QString& category, qreal x)

{

    if (m_categoryContentX.value(category, 0.0) != x) {

        m_categoryContentX[category] = x;

        

        // Only trigger visibility check when scrolling the current category

        if (category == m_currentCategory) {

            handleContentPositionChange();

        }

        

        update();

    }

}


qreal CustomImageListView::getCategoryContentX(const QString& category) const

{

    return m_categoryContentX.value(category, 0.0);

}


void CustomImageListView::updateCurrentCategory()

{

    if (m_currentIndex >= 0 && m_currentIndex < m_imageData.size()) {

        m_currentCategory = m_imageData[m_currentIndex].category;

    }

}


// Add safety check helper method

void CustomImageListView::ensureValidIndex(int &index) 

{

    if (index < 0) {

        index = 0;

    }

    if (index >= m_imageData.size()) {

        index = m_imageData.size() - 1;

    }

}


// Add in constructor after m_networkManager initialization

void CustomImageListView::setupNetworkManager()

{

    if (!m_networkManager) return;

    

    // Configure network settings for embedded systems

    m_networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

    

    // Enable SSL/HTTPS support

    #ifndef QT_NO_SSL

        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();

        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);  // Required for self-signed certs

        sslConfig.setProtocol(QSsl::TlsV1_0);  // Use TLS 1.0 for maximum compatibility

        QSslConfiguration::setDefaultConfiguration(sslConfig);

    #endif

}



void CustomImageListView::setupScrollAnimation()

{

    // Create a reusable animation object

    m_scrollAnimation = new QPropertyAnimation(this);

    m_scrollAnimation->setDuration(300);  // 300ms duration

    m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);

}



void CustomImageListView::animateScroll(const QString& category, qreal targetX)

{

    // Stop any existing animation

    stopCurrentAnimation();

    

    // Create a new animation for this category if it doesn't exist

    if (!m_categoryAnimations.contains(category)) {

        QPropertyAnimation* anim = new QPropertyAnimation(this);

        anim->setDuration(300);

        anim->setEasingCurve(QEasingCurve::OutCubic);

        m_categoryAnimations[category] = anim;

        

        // Connect animation to update the scroll position

        connect(anim, &QPropertyAnimation::valueChanged, this, [this, category](const QVariant& value) {

            setCategoryContentX(category, value.toReal());

        });

    }

    

    QPropertyAnimation* anim = m_categoryAnimations[category];

    anim->stop();

    anim->setStartValue(getCategoryContentX(category));

    anim->setEndValue(targetX);

    anim->start();

}



void CustomImageListView::stopCurrentAnimation()

{

    // Stop all category animations

    for (QPropertyAnimation* anim : m_categoryAnimations.values()) {

        if (anim->state() == QPropertyAnimation::Running) {

            anim->stop();

        }

    }

}



void CustomImageListView::setStartPositionX(qreal x)

{

    if (m_startPositionX != x) {

        m_startPositionX = x;

        emit startPositionXChanged();

        update();

    }

}



void CustomImageListView::setEnableNodeMetrics(bool enable)

{

    if (m_enableNodeMetrics != enable) {

        m_enableNodeMetrics = enable;

        emit enableNodeMetricsChanged();

        update();

    }

}



void CustomImageListView::setEnableTextureMetrics(bool enable)

{

    if (m_enableTextureMetrics != enable) {

        m_enableTextureMetrics = enable;

        emit enableTextureMetricsChanged();

        update();

    }

}



void CustomImageListView::safeCleanup()

{

    // Use a static guard to prevent reentrant calls

    static bool cleaningInProgress = false;

    if (cleaningInProgress) {

        return; // Prevent recursive cleanup

    }

    cleaningInProgress = true;

    

    // First mark destruction flag to prevent further rendering

    m_isBeingDestroyed = true;

    

    // Critical step: make item invisible to prevent it from being rendered

    setVisible(false);

    setEnabled(false);

    

    // Unregister from render thread by ensuring item isn't part of scene graph

    QQuickWindow* win = window();

    if (win) {

        // Use Qt 5.6 compatible syntax

        disconnect(win, SIGNAL(beforeRendering()), this, nullptr);

        disconnect(win, SIGNAL(afterRendering()), this, nullptr);

        disconnect(win, SIGNAL(sceneGraphInitialized()), this, nullptr);

        

        // Force a sync/render cycle to apply visibility change before we touch the nodes

        win->update();

        

        // For Qt 5.6, we need to ensure the update is processed

        QCoreApplication::processEvents();

    }

    

    // Stop any pending animations first - they could trigger updates

    if (m_scrollAnimation) {

        m_scrollAnimation->stop();

        delete m_scrollAnimation;

        m_scrollAnimation = nullptr;

    }



    // Clear category animations

    QList<QPropertyAnimation*> animations = m_categoryAnimations.values();

    m_categoryAnimations.clear();

    for (QPropertyAnimation* anim : animations) {

        anim->stop();

        delete anim;

    }



    // Handle network cleanup - collect replies first

    QList<QNetworkReply*> pendingReplies;

    {

        QMutexLocker networkLocker(&m_networkMutex);

        pendingReplies = m_pendingRequests.values();

        m_pendingRequests.clear();

    }



    // Process replies outside the lock

    for (QNetworkReply* reply : pendingReplies) {

        if (reply) {

            reply->disconnect();

            reply->abort();

            reply->deleteLater();

        }

    }



    // Clear URL cache to free memory

    m_urlImageCache.clear();



    // Add memory barrier before texture operations to ensure 

    // rendering thread isn't accessing textures

    QCoreApplication::processEvents();



    // Critical section for safe node handling

    {

        QMutexLocker locker(&m_loadMutex);

        

        // Create a list of nodes to be safely deleted

        QList<QSGNode*> nodesToDelete;

        

        // First collect all nodes to be deleted

        for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it) {

            if (it.value().node) {

                nodesToDelete.append(it.value().node);

                // Just clear pointer immediately to prevent any further access

                it.value().node = nullptr;

            }

            // Just nullify the texture pointer (window owns the texture)

            it.value().texture = nullptr;

        }

        

        // Schedule deletion of collected nodes when safe

        if (win && !nodesToDelete.isEmpty()) {

            // Schedule a safe batch deletion job

            win->scheduleRenderJob(new SafeNodeBatchDeleter(nodesToDelete),

                                 QQuickWindow::BeforeRenderingStage);

        }

        

        m_nodes.clear();

    }

    

    // Clear data structures that might be accessed from other methods

    m_imageData.clear();

    m_rowTitles.clear();

    m_categoryContentX.clear();

    

    // Reset tracking state

    cleaningInProgress = false;

}



void CustomImageListView::onNetworkReplyFinished()

{

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) return;

    

    int index = -1;

    

    // Get the index for this reply with minimal lock time

    {

        QMutexLocker locker(&m_networkMutex);

        index = m_pendingRequests.key(reply, -1);

        

        // Remove from pending requests while under lock

        if (index != -1) {

            m_pendingRequests.remove(index);

        }

    }

    

    // Process the reply outside of mutex lock to avoid deadlocks

    if (index == -1) {

        reply->deleteLater();

        return;

    }



    if (reply->error() == QNetworkReply::NoError) {

        QByteArray data = reply->readAll();

        if (!data.isEmpty()) {

            QImage image;

            if (image.loadFromData(data)) {

                m_urlImageCache.insert(reply->url(), image);

                processLoadedImage(index, image);

            } else {

                createFallbackTexture(index);

            }

        } else {

            createFallbackTexture(index);

        }

    } else {

        createFallbackTexture(index);

    }



    reply->deleteLater();

}



void CustomImageListView::onNetworkError(QNetworkReply::NetworkError code)

{

    Q_UNUSED(code); // Silence unused parameter warning

    

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) return;

    

    int index = -1;

    

    // Find the index with minimal lock time

    {

        QMutexLocker locker(&m_networkMutex);

        index = m_pendingRequests.key(reply, -1);

        if (index != -1) {

            m_pendingRequests.remove(index);

        }

    }

    

    // Process outside the lock

    if (index != -1) {

        createFallbackTexture(index);

    }

    

    reply->deleteLater();

}



void CustomImageListView::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)

{

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) return;



    int index = -1;

    

    {

        QMutexLocker locker(&m_networkMutex);

        index = m_pendingRequests.key(reply, -1);

    }

    

    if (index != -1) {

        qDebug() << "Download progress for index" << index << ":"

                 << bytesReceived << "/" << bytesTotal << "bytes";

    }

}



void CustomImageListView::onScrollAnimationValueChanged(const QVariant &value)

{

    QPropertyAnimation *anim = qobject_cast<QPropertyAnimation*>(sender());

    if (!anim) return;

    

    // Get the category associated with this animation

    QString category = anim->property("category").toString();

    if (category.isEmpty()) return;

    

    // Update the category scroll position

    setCategoryContentX(category, value.toReal());

}



void CustomImageListView::updateMetricCounts(int nodes, int textures)

{

    if (m_totalNodeCount != nodes || m_textureCount != textures) {

        m_totalNodeCount = nodes;

        m_textureCount = textures;

        // Emit debug information

        qDebug() << "Scene Graph Metrics Updated - Nodes:" << m_totalNodeCount 

                 << "Textures:" << m_textureCount;

    }

}



CustomImageListView::CategoryDimensions CustomImageListView::getDimensionsForCategory(const QString& category) const

{

    // Check if we have custom dimensions for this category

    if (m_categoryDimensions.contains(category)) {

        return m_categoryDimensions.value(category);

    }

    

    // Return default dimensions for categories without custom settings

    CategoryDimensions defaultDims;

    defaultDims.rowHeight = m_itemHeight + 10; // Add some padding

    defaultDims.posterHeight = m_itemHeight;

    defaultDims.posterWidth = m_itemWidth;

    defaultDims.itemSpacing = m_spacing;

    

    return defaultDims;

}



int CustomImageListView::countNodes(QSGNode *root)

{

    if (!root) {

        return 0;

    }



    int total = 1; // Count the current node

    for (QSGNode *child = root->firstChild(); child; child = child->nextSibling()) {

        total += countNodes(child);

    }

    return total;

}



int CustomImageListView::countTotalTextures(QSGNode *root)

{

    QSet<QSGTexture *> textures;

    collectTextures(root, textures);

    return textures.size();

}



void CustomImageListView::collectTextures(QSGNode *node, QSet<QSGTexture *> &textures)

{

    if (!node) return;



    // Check geometry node materials

    if (node->type() == QSGNode::GeometryNodeType) {

        QSGGeometryNode *geometryNode = static_cast<QSGGeometryNode*>(node);

        QSGMaterial *mat = geometryNode->activeMaterial();

        if (mat) {

            // QSGTextureMaterial

            QSGTextureMaterial *texMat = dynamic_cast<QSGTextureMaterial*>(mat);

            if (texMat && texMat->texture()) {

                textures.insert(texMat->texture());

            }

            // QSGOpaqueTextureMaterial

            QSGOpaqueTextureMaterial *opaqueTexMat = dynamic_cast<QSGOpaqueTextureMaterial*>(mat);

            if (opaqueTexMat && opaqueTexMat->texture()) {

                textures.insert(opaqueTexMat->texture());

            }

        }

    }



    // Check QSGSimpleTextureNode

    QSGSimpleTextureNode *simpleTex = dynamic_cast<QSGSimpleTextureNode*>(node);

    if (simpleTex && simpleTex->texture()) {

        textures.insert(simpleTex->texture());

    }



    // Recurse through children

    for (QSGNode *child = node->firstChild(); child; child = child->nextSibling()) {

        collectTextures(child, textures);

    }

}



void CustomImageListView::recycleOffscreenTextures()

{

    QMutexLocker locker(&m_loadMutex);

    

    QList<int> indicesToRemove;

    

    // First pass: mark textures from non-visible categories for removal

    for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it) {

        int index = it.key();

        if (index < m_imageData.size()) {

            QString category = m_imageData[index].category;

            if (!m_visibleCategories.contains(category)) {

                indicesToRemove.append(index);

            }

        }

    }

    

    // Second pass: if we still have too many textures, remove oldest non-visible ones

    if (m_nodes.size() > CLEANUP_THRESHOLD) {

        for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it) {

            int index = it.key();

            if (!m_visibleIndices.contains(index) && !indicesToRemove.contains(index)) {

                // Check if this texture is from a visible category but off-screen

                QString category = m_imageData[index].category;

                if (m_visibleCategories.contains(category)) {

                    // Keep textures close to visible area

                    CategoryDimensions dims = getDimensionsForCategory(category);

                    qreal itemX = getCategoryContentX(category);

                    if (itemX < -dims.posterWidth * 2 || itemX > width() + dims.posterWidth * 2) {

                        indicesToRemove.append(index);

                    }

                }

            }

        }

    }

    

    // Remove marked textures

    for (int index : indicesToRemove) {

        cleanupNode(m_nodes[index]);

        m_nodes.remove(index);

        m_textureUsageQueue.removeOne(index);

    }

}



void CustomImageListView::cacheTexture(int index, QSGTexture* texture)

{

    // Update usage queue

    m_textureUsageQueue.removeAll(index);

    m_textureUsageQueue.enqueue(index);

    

    // Store texture

    TexturedNode node;

    node.texture = texture;

    node.node = nullptr;

    m_nodes[index] = node;

}



int CustomImageListView::getCurrentItemPositionInCategory() const

{

    // Return 0 if no valid current index

    if (m_currentIndex < 0 || m_currentIndex >= m_imageData.size()) {

        return 0;

    }



    // Get current category

    QString currentCategory = m_imageData[m_currentIndex].category;

    int position = 0;



    // Count items in same category up to current index

    for (int i = 0; i < m_currentIndex; i++) {

        if (m_imageData[i].category == currentCategory) {

            position++;

        }

    }



    return position;

}



// Add this helper method

bool CustomImageListView::shouldJumpToLastRow(int currentIndex) const

{

    if (currentIndex < 0 || currentIndex >= m_imageData.size()) {

        return false;

    }



    QString currentCategory = m_imageData[currentIndex].category;

    int categoryIndex = m_rowTitles.indexOf(currentCategory);

    

    // Get number of items in current category

    int totalItemsInCategory = 0;

    int currentPositionInCategory = 0;

    

    for (int i = 0; i < m_imageData.size(); i++) {

        if (m_imageData[i].category == currentCategory) {

            totalItemsInCategory++;

            if (i <= currentIndex) {

                currentPositionInCategory++;

            }

        }

    }

    

    // Check if we're past halfway point in the current row

    bool isPastHalfway = (currentPositionInCategory > totalItemsInCategory / 2);

    

    // Check if there's only one row remaining after current row

    bool isOneRowRemaining = (categoryIndex == m_rowTitles.size() - 2);

    

    return isPastHalfway && isOneRowRemaining;

}



void CustomImageListView::ensureLastRowVisible(int targetIndex)

{

    CategoryDimensions dims = getDimensionsForCategory(m_imageData[targetIndex].category);

    

    // Calculate the maximum possible scroll position

    qreal maxScroll = contentHeight() - height();

    

    // Always scroll to maximum position for last row

    qreal desiredY = maxScroll;

    

    // Add extra padding to ensure the entire row and focus effects are visible

    desiredY += dims.posterHeight * 0.1;  // Add 10% of poster height as padding

    

    // Apply animation with bounds checking

    QPropertyAnimation* anim = new QPropertyAnimation(this, "contentY");

    anim->setDuration(300);

    anim->setEasingCurve(QEasingCurve::OutCubic);

    anim->setStartValue(m_contentY);

    anim->setEndValue(desiredY);

    

    // Update selection only after animation completes

    connect(anim, &QPropertyAnimation::finished, this, [this, targetIndex]() {

        setCurrentIndex(targetIndex);

        ensureFocus();

        

        // Force a final position check

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

