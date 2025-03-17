#include "customimagelistview.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGTextureMaterial>
#include <QSGOpaqueTextureMaterial>
#include <QQuickWindow>
#include <QNetworkReply>
#include <QDebug>
#include <QSGSimpleTextureNode>
#include <QPainter>
#include <QFontMetrics>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QTimer>
#include <QSGFlatColorMaterial>
#include <cmath>
#include <QtMath>
#include "texturemanager.h"
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslSocket>
#include <QtNetwork/QSslConfiguration>
#include <QRunnable>  // Add this line to include QRunnable
#include <QPointer>
#include <QPainterPath>  // Add this include to fix QPainterPath error



//Q_LOGGING_CATEGORY(ihScheduleModel2, "custom", QtDebugMsg)

CustomImageListView::CustomImageListView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // Set up rendering flags
    setFlag(ItemHasContents, true);
    setFlag(QQuickItem::ItemIsFocusScope, true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFocus(true);
    setAcceptHoverEvents(true);
    setActiveFocusOnTab(true);

    // Set initial dimensions
    setHeight(300);
    setImplicitHeight(300);

    // Configure network manager
    setupNetworkManager();
    
    // Connect to window change signal with proper lambda capture
    connect(this, &QQuickItem::windowChanged, this, [this](QQuickWindow *w) {
        if (w) {
            // Capture the window pointer in the inner lambda
            connect(w, &QQuickWindow::beforeRendering, this, [this, w]() {
                if (!m_windowReady && w->isSceneGraphInitialized()) {
                    m_windowReady = true;
                    loadAllImages();
                }
            }, Qt::DirectConnection);
        }
    });
    
    // Initialize animation system
    setupScrollAnimation();
}

CustomImageListView::~CustomImageListView()
{
    // Mark as being destroyed first
    m_isBeingDestroyed = true;
    
    // Critical step: ensure scene graph isn't rendering our nodes
    QQuickWindow* win = window();
    if (win) {
        // Disconnect signals in Qt 5.6 compatible way
        disconnect(win, SIGNAL(beforeRendering()), this, nullptr);
        disconnect(win, SIGNAL(afterRendering()), this, nullptr);
        
        // Block scene graph updates for our item
        setVisible(false);
        setEnabled(false);
        
        // Force update to apply visibility change before cleanup
        win->update();
        
        // Wait for update to be processed
        QCoreApplication::processEvents();
    }
    
    // Safe cleanup with proper barriers
    safeCleanup();
}

// Fix #2: Proper initial positioning
void CustomImageListView::componentComplete()
{
    QQuickItem::componentComplete();
    
    // Reset all category scroll positions to 0
    m_categoryContentX.clear();
    
    // Reset vertical scroll position
    m_contentY = 0;
    
    // Reset all animations
    stopCurrentAnimation();
    
    // Initialize all row scroll positions to 0
    if (!m_rowTitles.isEmpty()) {
        for (const QString& category : m_rowTitles) {
            setCategoryContentX(category, 0);
        }
    }

    // Use a timer to ensure proper initial positioning after everything is loaded
    QTimer::singleShot(150, this, [this]() {
        // Set initial index if not set
        if (m_currentIndex < 0 || m_currentIndex >= m_imageData.size()) {
            // Default to the first item
            if (!m_imageData.isEmpty()) {
                m_currentIndex = 0;
            }
        }
        
        if (m_currentIndex >= 0 && m_currentIndex < m_imageData.size()) {
            // Update current category
            QString category = m_imageData[m_currentIndex].category;
            updateCurrentCategory();
            
            // Make sure the item is centered both horizontally and vertically
            qreal viewportCenter = height() / 2;
            qreal targetY = calculateItemVerticalPosition(m_currentIndex);
            CategoryDimensions dims = getDimensionsForCategory(category);
            qreal initialScrollY = targetY - viewportCenter + (dims.rowHeight / 2);
            
            // Set the initial scroll position
            setContentY(qMax(0.0, initialScrollY));
            
            // Set the horizontal position too (center the focused item)
            int itemPosInCategory = getCurrentItemPositionInCategory();
            qreal targetX = itemPosInCategory * (dims.posterWidth + dims.itemSpacing);
            targetX = qMax(0.0, targetX - (width() - dims.posterWidth) / 2);
            setCategoryContentX(category, targetX);
            
            // Force loading of visible textures
            m_visibleCategories.insert(category);
            loadVisibleTextures();
            
            // Update and ensure focus
            update();
            ensureFocus();
        }
    });
    
    // Initial starting position
    m_startPositionX = 50;  // Fixed left margin of 50px
    
    update();
}

void CustomImageListView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    // Only reload when we have valid dimensions and window
    if (newGeometry.width() > 0 && newGeometry.height() > 0 && window()) {
        // Delay loading slightly to ensure window is ready
        QTimer::singleShot(100, this, [this]() {
            loadAllImages();
        });
    }
}

// Replace the loadAllImages method with this improved version
void CustomImageListView::loadAllImages()
{
    if (!isReadyForTextures()) {
        QTimer::singleShot(100, this, [this]() {
            loadAllImages();
        });
        return;
    }

    // Get current visible indices - use the parameterized version with default buffer
    QVector<int> newVisibleIndices = getVisibleIndices(0.5);
    
    // Update visible set
    m_visibleIndices.clear();
    for (int index : newVisibleIndices) {
        m_visibleIndices.insert(index);
    }
    
    // Only load visible images immediately
    for (int index : newVisibleIndices) {
        if (!m_nodes.contains(index)) {
            loadImage(index);
        }
    }
    
    // Cleanup old textures if we have too many
    if (m_nodes.size() > CLEANUP_THRESHOLD) {
        recycleOffscreenTextures();
    }
}

// Fix the handleContentPositionChange method to use the parameterized version
void CustomImageListView::handleContentPositionChange()
{
    if (m_isBeingDestroyed) return;
    
    // Update visible categories with expanded range
    m_visibleCategories.clear();
    qreal bufferSize = height() * 0.5;
    qreal viewportTop = m_contentY - bufferSize;
    qreal viewportBottom = m_contentY + height() + bufferSize;
    qreal currentY = 0;
    
    // Determine which categories are visible
    for (const QString &category : m_rowTitles) {
        CategoryDimensions dims = getDimensionsForCategory(category);
        qreal rowStart = currentY;
        qreal rowEnd = currentY + dims.rowHeight + m_titleHeight;
        
        // Check if this row is visible
        if (!(rowEnd < viewportTop || rowStart > viewportBottom)) {
            m_visibleCategories.insert(category);
        }
        
        currentY += dims.rowHeight + m_titleHeight + m_rowSpacing;
    }
    
    // Get visible indices as before - use the parameterized version with default buffer
    QVector<int> newVisibleIndices = getVisibleIndices(0.5);
    m_visibleIndices.clear();
    for (int index : newVisibleIndices) {
        m_visibleIndices.insert(index);
    }
    
    // Load textures for newly visible items
    for (int index : newVisibleIndices) {
        if (!m_nodes.contains(index)) {
            QTimer::singleShot(10, this, [this, index]() {
                if (!m_isBeingDestroyed) {
                    loadImage(index);
                }
            });
        }
    }
    
    // Cleanup if we have too many textures
    if (m_nodes.size() > CLEANUP_THRESHOLD) {
        recycleOffscreenTextures();
    }
}

// Add this new method to determine which indices are visible
// Delete or comment out this function since we'll use the parameterized version
/*
QVector<int> CustomImageListView::getVisibleIndices()
{
    // ... existing implementation ...
}
*/

void CustomImageListView::safeReleaseTextures()
{
    QMutexLocker locker(&m_loadMutex);
    
    // Only delete nodes, not textures
    for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it) {
        if (it.value().node) {
            delete it.value().node;
        }
    }
    m_nodes.clear();
}


void CustomImageListView::setCount(int count)
{
    if (m_count != count) {
        m_count = count;
        for(int i = 0; i < m_count; i++) {
            loadImage(i);
        }
        emit countChanged();
        update();
    }
}

void CustomImageListView::setItemWidth(qreal width)
{
    if (m_itemWidth != width) {
        m_itemWidth = width;
        emit itemWidthChanged();
        update();
    }
}

void CustomImageListView::setItemHeight(qreal height)
{
    if (m_itemHeight != height) {
        m_itemHeight = height;
        emit itemHeightChanged();
        update();
    }
}

void CustomImageListView::setSpacing(qreal spacing)
{
    if (m_spacing != spacing) {
        m_spacing = spacing;
        emit spacingChanged();
        update();
    }
}

void CustomImageListView::setRowSpacing(qreal spacing)
{
    if (m_rowSpacing != spacing) {
        m_rowSpacing = spacing;
        emit rowSpacingChanged();
        update();
    }
}

void CustomImageListView::setUseLocalImages(bool local)
{
    if (m_useLocalImages != local) {
        m_useLocalImages = local;
        emit useLocalImagesChanged();
        loadAllImages();
    }
}

void CustomImageListView::setImagePrefix(const QString &prefix)
{
    if (m_imagePrefix != prefix) {
        m_imagePrefix = prefix;
        emit imagePrefixChanged();
        if (m_useLocalImages) {
            loadAllImages();
        }
    }
}

void CustomImageListView::setImageUrls(const QVariantList &urls)
{
    if (m_imageUrls != urls) {
        m_imageUrls = urls;
        emit imageUrlsChanged();
        if (m_useLocalImages) {
            loadAllImages();
        }
    }
}

QString CustomImageListView::generateImageUrl(int index) const
{
    // Ensure minimum dimensions
    int imgWidth = qMax(int(m_itemWidth), 100);
    int imgHeight = qMax(int(height()), 100);
    
    QString url = QString("http://dummyimage.com/%1x%2/000/fff.jpg&text=img%3")
            .arg(imgWidth)
            .arg(imgHeight)
            .arg(index + 1);
            
    return url;
}

QImage CustomImageListView::loadLocalImage(int index) const
{
    QString imagePath;
    // Ensure index is within bounds of available images
    int actualIndex = (index % 5) + 1; // We have img1.jpg through img5.jpg
    
    // Update path to match new structure
    imagePath = QString(":/data/images/img%1.jpg").arg(actualIndex);
    
    qDebug() << "\nTrying to load image at index:" << index;
    qDebug() << "Using actual image path:" << imagePath;

    QFile file(imagePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray imageData = file.readAll();
        QImage image;
        if (image.loadFromData(imageData)) {
            qDebug() << "Successfully loaded image from:" << imagePath;
            qDebug() << "Image size:" << image.size();
            file.close();
            return image;
        }
        file.close();
    }

    // Create fallback image
    QImage fallback(m_itemWidth, m_itemHeight, QImage::Format_ARGB32);
    fallback.fill(Qt::red);
    QPainter painter(&fallback);
    painter.setPen(Qt::white);
    painter.drawText(fallback.rect(), Qt::AlignCenter, 
                    QString("Failed\nImage %1").arg(index + 1));
    return fallback;
}

QSGGeometryNode* CustomImageListView::createTexturedRect(const QRectF &rect, QSGTexture *texture, bool isFocused)
{
    // Calculate scale factor for focus effect
    const float scaleFactor = isFocused ? 1.1f : 1.0f;  // 10% larger when focused
    
    // Calculate scaled dimensions and center position
    QRectF scaledRect = rect;
    if (isFocused) {
        qreal widthDiff = rect.width() * (scaleFactor - 1.0f);
        qreal heightDiff = rect.height() * (scaleFactor - 1.0f);
        scaledRect = QRectF(
            rect.x() - widthDiff / 2,
            rect.y() - heightDiff / 2,
            rect.width() * scaleFactor,
            rect.height() * scaleFactor
        );
    }

    // Create geometry for a textured rectangle
    QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
    geometry->setDrawingMode(GL_TRIANGLE_STRIP);

    QSGGeometry::TexturedPoint2D *vertices = geometry->vertexDataAsTexturedPoint2D();
    
    // Set vertex positions using scaled rect
    vertices[0].set(scaledRect.left(), scaledRect.top(), 0.0f, 0.0f);
    vertices[1].set(scaledRect.right(), scaledRect.top(), 1.0f, 0.0f);
    vertices[2].set(scaledRect.left(), scaledRect.bottom(), 0.0f, 1.0f);
    vertices[3].set(scaledRect.right(), scaledRect.bottom(), 1.0f, 1.0f);

    QSGGeometryNode *node = new QSGGeometryNode;
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);

    // For rounded corners, use textured material with alpha blending instead of opaque
    QSGTextureMaterial *material = new QSGTextureMaterial;
    material->setTexture(texture);
    
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsMaterial);

    return node;
}

// Add this helper method at the beginning of the class implementation
bool CustomImageListView::isReadyForTextures() const
{
    if (!m_windowReady || !window()) {
        return false;
    }
    
    // Check if window is exposed and visible
    if (!window()->isVisible() || !window()->isExposed()) {
        return false;
    }
    
    // Check window size
    if (window()->width() <= 0 || window()->height() <= 0) {
        return false;
    }
    
    return true;
}

void CustomImageListView::loadImage(int index)
{
    if (m_isBeingDestroyed || !ensureValidWindow() || index >= m_imageData.size()) {
        return;
    }

    QMutexLocker locker(&m_loadMutex);
    
    if (!isReadyForTextures()) {
        QTimer::singleShot(100, this, [this, index]() {
            loadImage(index);
        });
        return;
    }

    // Prevent duplicate texture creation
    if (m_nodes.contains(index) && m_nodes[index].texture) {
        return;
    }

    m_isLoading = true;

    // Load from URL
    const ImageData &imgData = m_imageData[index];
    QString imagePath = imgData.url;

    // First try to load as local resource
    QImage image = loadLocalImageFromPath(imagePath);
    if (!image.isNull()) {
        processLoadedImage(index, image);
        m_isLoading = false;
        return;
    }

    // If not a local resource, try as network URL
    QUrl url(imagePath);
    if (url.scheme().startsWith("http") || imagePath.startsWith("//")) {
        // Convert // URLs to http://
        if (imagePath.startsWith("//")) {
            url = QUrl("http:" + imagePath);
        }
        loadUrlImage(index, url);
    } else {
        createFallbackTexture(index);
    }

    m_isLoading = false;
}

bool CustomImageListView::ensureValidWindow() const
{
    return window() && window()->isExposed() && !m_isDestroying;
}

void CustomImageListView::createFallbackTexture(int index)
{
    // Create a basic RGB texture
    QImage fallback(m_itemWidth, m_itemHeight, QImage::Format_RGB32);
    fallback.fill(Qt::darkGray);
    
    QPainter painter(&fallback);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(fallback.rect(), Qt::AlignCenter, 
                    QString("Image %1").arg(index + 1));
    painter.end();
    
    if (window()) {
        QSGTexture *texture = window()->createTextureFromImage(fallback);
        if (texture) {
            TexturedNode node;
            node.texture = texture;
            node.node = nullptr;
            m_nodes[index] = node;
            update();
            qDebug() << "Created fallback texture for index:" << index;
        }
    }
}

QImage CustomImageListView::loadLocalImageFromPath(const QString &path) const
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray imageData = file.readAll();
        QImage image;
        if (image.loadFromData(imageData)) {
            qDebug() << "Successfully loaded image from:" << path;
            qDebug() << "Image size:" << image.size();
            file.close();
            return image;
        }
        file.close();
    }

    qDebug() << "Failed to load image from:" << path;
    return QImage();
}

// Update loadUrlImage method to better handle HTTP requests
void CustomImageListView::loadUrlImage(int index, const QUrl &url)
{
    if (!url.isValid()) {
        qWarning() << "Invalid URL:" << url.toString();
        createFallbackTexture(index);
        return;
    }

    // Convert URL if needed (for URLs starting with //)
    QUrl finalUrl = url;
    if (url.toString().startsWith("//")) {
        finalUrl = QUrl("http:" + url.toString());
    }

    // For HTTP URLs
    if (finalUrl.scheme() == "http") {
        qDebug() << "Loading image" << index << "from URL:" << finalUrl.toString();
        
        // Create network request
        QNetworkRequest request(finalUrl);
        
        // Essential headers for Qt 5.6
        request.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; Qt/5.6)");
        request.setRawHeader("Accept", "image/webp,image/apng,image/*,*/*;q=0.8");
        request.setRawHeader("Connection", "keep-alive");
        
        // Enable redirect following
        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        
        QNetworkReply* oldReply = nullptr;
        
        // Only lock mutex for the map operations
        {
            QMutexLocker locker(&m_networkMutex);
            
            // Store old reply for later deletion outside the lock
            if (m_pendingRequests.contains(index)) {
                oldReply = m_pendingRequests.take(index);
            }
        }
        
        // Cancel old request outside the mutex lock
        if (oldReply) {
            oldReply->disconnect(); // Prevent callbacks
            oldReply->abort();
            oldReply->deleteLater();
        }
      
      
    #ifndef QT_NO_SSL
        if (finalUrl.scheme() == "https") {
            QSslConfiguration sslConfig = request.sslConfiguration();
            sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
            sslConfig.setProtocol(QSsl::TlsV1_0);
            request.setSslConfiguration(sslConfig);
        }
    #endif
          
        // Create network reply
        QNetworkReply *reply = m_networkManager->get(request);
        
        // Store the reply in our map with minimal lock time
        {
            QMutexLocker locker(&m_networkMutex);
            m_pendingRequests[index] = reply;
        }
        
        // Connect signals with Qt 5.6 compatible syntax
        connect(reply, SIGNAL(finished()), this, SLOT(onNetworkReplyFinished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), 
                this, SLOT(onNetworkError(QNetworkReply::NetworkError)));
        connect(reply, SIGNAL(sslErrors(QList<QSslError>)), 
                reply, SLOT(ignoreSslErrors()));
        
        // Add longer timeout for slower connections
        QTimer::singleShot(30000, reply, SLOT(abort()));
    } else {
        qWarning() << "Unsupported URL scheme:" << finalUrl.scheme();
        createFallbackTexture(index);

    }

}

void CustomImageListView::processLoadedImage(int index, const QImage &image)
{
    if (!image.isNull() && window() && m_visibleIndices.contains(index)) {
        QString category = "";
        if (index < m_imageData.size()) {
            category = m_imageData[index].category;
        }
        
        CategoryDimensions dims = getDimensionsForCategory(category);
        QImage::Format format = QImage::Format_ARGB32_Premultiplied;
        
        // Convert image to optimal format if needed
        QImage optimizedImage = (image.format() == format) ? 
                               image : image.convertToFormat(format);
        
        // Create target image with transparent background
        QImage roundedImage(dims.posterWidth, dims.posterHeight, format);
        roundedImage.fill(Qt::transparent);
        
        QPainter painter(&roundedImage);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        
        // Calculate corner radius
        int radius = qMin(dims.posterWidth, dims.posterHeight) * 0.1;
        radius = qBound(5, radius, 15);
        
        // Create rounded rectangle path
        QPainterPath path;
        path.addRoundedRect(0, 0, dims.posterWidth, dims.posterHeight, radius, radius);
        painter.setClipPath(path);
        
        // Calculate scaling while preserving aspect ratio
        qreal sourceRatio = (qreal)optimizedImage.width() / optimizedImage.height();
        qreal targetRatio = (qreal)dims.posterWidth / dims.posterHeight;
        
        QRect targetRect;
        if (sourceRatio > targetRatio) {
            // Image is wider than target - scale to height
            int scaledWidth = dims.posterHeight * sourceRatio;
            targetRect = QRect((dims.posterWidth - scaledWidth) / 2, 0, 
                             scaledWidth, dims.posterHeight);
        } else {
            // Image is taller than target - scale to width
            int scaledHeight = dims.posterWidth / sourceRatio;
            targetRect = QRect(0, (dims.posterHeight - scaledHeight) / 2,
                             dims.posterWidth, scaledHeight);
        }
        
        // Draw the image scaled to fit
        painter.drawImage(targetRect, optimizedImage);
        
        // Add subtle border
        painter.setClipping(false);
        painter.setPen(QPen(QColor(255, 255, 255, 40), 1));
        painter.drawPath(path);
        
        painter.end();
        
        // Create texture
        QSGTexture *texture = window()->createTextureFromImage(
            roundedImage,
            QQuickWindow::TextureHasAlphaChannel
        );

        if (texture) {
            cacheTexture(index, texture);
            update();
        }
    }
}

void CustomImageListView::setImageTitles(const QStringList &titles)
{
    if (m_imageTitles != titles) {
        m_imageTitles = titles;
        emit imageTitlesChanged();
        update();
    }
}

void CustomImageListView::setRowTitles(const QStringList &titles)
{
    if (m_rowTitles != titles) {
        m_rowTitles = titles;
        emit rowTitlesChanged();
        update();
    }
}

QSGGeometryNode* CustomImageListView::createOptimizedTextNode(const QString &text, const QRectF &rect)
{
    if (!window()) {
        return nullptr;
    }

    // Create new texture for text (don't use caching for now)
    int width = qMax(1, static_cast<int>(std::ceil(rect.width())));
    int height = qMax(1, static_cast<int>(std::ceil(rect.height())));
    
    QImage textImage(width, height, QImage::Format_ARGB32_Premultiplied);
    if (textImage.isNull()) {
        return nullptr;
    }
    
    textImage.fill(QColor(0, 0, 0, 0));
    
    QPainter painter;
    if (!painter.begin(&textImage)) {
        return nullptr;
    }

    static const QFont itemFont("Arial", 12);
    painter.setFont(itemFont);
    painter.setPen(Qt::white);
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter.drawText(textImage.rect(), Qt::AlignCenter, text);
    painter.end();
    
    QSGTexture *texture = window()->createTextureFromImage(
        textImage,
        QQuickWindow::TextureHasAlphaChannel
    );
    
    return texture ? createTexturedRect(rect, texture) : nullptr;
}

QSGGeometryNode* CustomImageListView::createRowTitleNode(const QString &text, const QRectF &rect)
{
    if (!window()) {
        return nullptr;
    }

    // Calculate title width based on text content
    QFont titleFont("Roboto", 20, QFont::Bold);  // Using Roboto font, size 20, bold
    QFontMetrics fm(titleFont);
    int textWidth = fm.width(text) + 20;  // Add 20px padding
    int width = qMax(textWidth, static_cast<int>(std::ceil(rect.width())));
    int height = qMax(1, static_cast<int>(std::ceil(rect.height())));

    QImage textImage(width, height, QImage::Format_ARGB32_Premultiplied);
    if (textImage.isNull()) {
        return nullptr;
    }

    textImage.fill(QColor(0, 0, 0, 0));

    QPainter painter;
    if (!painter.begin(&textImage)) {
        return nullptr;
    }

    // Configure text rendering with focus properties
    painter.setFont(titleFont);
    painter.setPen(QColor("#ebebeb"));  // Set focus color
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    // Draw text with left alignment and vertical centering
    int yPos = (height + fm.ascent() - fm.descent()) / 2;
    painter.drawText(10, yPos, text);
    
    painter.end();

    // Create texture
    QSGTexture *texture = window()->createTextureFromImage(
        textImage,
        QQuickWindow::TextureHasAlphaChannel
    );

    if (!texture) {
        return nullptr;
    }

    // Create adjusted rect with 8 pixels left offset
    QRectF adjustedRect(rect.x() - 8, rect.y(), textWidth, rect.height());
    return createTexturedRect(adjustedRect, texture);
}

// Fix the return type from void to QSGNode*
QSGNode* CustomImageListView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // Return null immediately if we're being destroyed to prevent render thread from accessing nodes
    if (m_isBeingDestroyed || !window() || !window()->isExposed()) {
        if (oldNode) {
            // Use a safer way to delete the node tree that won't crash during render
            oldNode->markDirty(QSGNode::DirtySubtreeBlocked); // Block rendering of this subtree
            
            // Schedule node deletion for next frame when render thread is done with it
            QQuickWindow *win = window();
            if (win) {
                // Create and schedule a SafeNodeDeleter to handle cleanup
                win->scheduleRenderJob(new SafeNodeDeleter(oldNode), 
                                     QQuickWindow::BeforeSynchronizingStage);
            }
            
            // Don't delete here - let the scene graph handle it
            return nullptr;
        }
        return nullptr;
    }
    
    QSGNode *parentNode = oldNode ? oldNode : new QSGNode;
    
    // Clear old nodes safely
    while (parentNode->childCount() > 0) {
        QSGNode *node = parentNode->firstChild();
        if (node) {
            parentNode->removeChildNode(node);
            delete node;
        }
    }
    
    qreal currentY = -m_contentY;
    int currentImageIndex = 0;
    
    // Create category containers
    for (int categoryIndex = 0; categoryIndex < m_rowTitles.size(); ++categoryIndex) {
        QString categoryName = m_rowTitles[categoryIndex];
        
        // Get dimensions for this category
        CategoryDimensions dims = getDimensionsForCategory(categoryName);
        
        // Calculate title width based on category name
        static const QFont titleFont("Arial", 24, QFont::Bold);
        QFontMetrics fm(titleFont);
        int titleWidth = fm.width(categoryName) + 20;  // Add 20px padding
        
        // Add startPositionX to title position
        QRectF titleRect(m_startPositionX + 10, currentY, titleWidth, m_titleHeight);  // Changed from 5 to 10
        QSGGeometryNode *titleNode = createRowTitleNode(categoryName, titleRect);
        if (titleNode) {
            parentNode->appendChildNode(titleNode);
        }
        currentY += m_titleHeight + 3; // Reduced from 5 to 3
        
        // Calculate items in this category
        QVector<ImageData> categoryItems;
        for (const ImageData &imgData : m_imageData) {
            if (imgData.category == categoryName) {
                categoryItems.append(imgData);
            }
        }

        // Add items using category-specific dimensions with 10-pixel offset (increased from 5)
        qreal xPos = m_startPositionX + 10 - getCategoryContentX(categoryName);  // Changed from 5 to 10
        for (const ImageData &imgData : categoryItems) {
            if (currentImageIndex < m_count) {
                QRectF rect(xPos, currentY, dims.posterWidth, dims.posterHeight);
                
                // Create item container
                QSGNode* itemContainer = new QSGNode;

                // Add image with focus effect
                if (m_nodes.contains(currentImageIndex) && m_nodes[currentImageIndex].texture) {
                    bool isFocused = (currentImageIndex == m_currentIndex);
                    QSGGeometryNode *imageNode = createTexturedRect(
                        rect, 
                        m_nodes[currentImageIndex].texture,
                        isFocused
                    );
                    if (imageNode) {
                        itemContainer->appendChildNode(imageNode);
                        m_nodes[currentImageIndex].node = imageNode;
                    }
                }

                // Add selection/focus effects if this is the current item
                if (currentImageIndex == m_currentIndex) {
                    addSelectionEffects(itemContainer, rect);
                }

                // Add title overlay
                //addTitleOverlay(itemContainer, rect, imgData.title);

                // Add the container to parent
                parentNode->appendChildNode(itemContainer);
                currentImageIndex++;
                xPos += dims.posterWidth + dims.itemSpacing;
            }
        }
        
        currentY += dims.rowHeight + (m_rowSpacing * 0.3); // Reduced from 0.75 to 0.3
    }
    
    // Before returning, update metrics with accurate counts
    if (m_enableNodeMetrics) {
        int realNodeCount = countNodes(parentNode);
        qDebug() << "Scene graph node metrics - Nodes:" << realNodeCount;
        
        if (m_enableTextureMetrics) {
            int realTextureCount = countTotalTextures(parentNode);
            qDebug() << "Scene graph texture metrics - Textures:" << realTextureCount;
            updateMetricCounts(realNodeCount, realTextureCount);
        } else {
            
            updateMetricCounts(realNodeCount, 0);
        }
    }
    
    return parentNode;
}

// Helper method for selection effects
void CustomImageListView::addSelectionEffects(QSGNode* container, const QRectF& rect)
{
    if (m_isBeingDestroyed) return;
    
    // Calculate scaled dimensions for focus border to match zoomed asset
    const float scaleFactor = 1.1f;
    qreal widthDiff = rect.width() * (scaleFactor - 1.0f);
    qreal heightDiff = rect.height() * (scaleFactor - 1.0f);
    
    // Ensure integer coordinates to avoid anti-aliasing artifacts
    QRectF scaledRect(
        qRound(rect.x() - widthDiff / 2),
        qRound(rect.y() - heightDiff / 2),
        qRound(rect.width() * scaleFactor),
        qRound(rect.height() * scaleFactor)
    );
    
    // Create geometry for rounded border
    QSGGeometryNode *borderNode = new QSGGeometryNode;
    
    const int segments = 32;
    // Add extra vertices to properly close the path
    const int totalPoints = ((segments + 1) * 4) + 2;
    
    QSGGeometry *borderGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), totalPoints);
    borderGeometry->setDrawingMode(GL_LINE_STRIP);
    borderGeometry->setLineWidth(1.5);
    
    QSGGeometry::Point2D *vertices = borderGeometry->vertexDataAsPoint2D();
    int idx = 0;
    
    qreal radius = qMin(scaledRect.width(), scaledRect.height()) * 0.1;
    radius = qBound(5.0, radius, 15.0);
    
    // Start exactly at left edge, middle of top-left radius
    vertices[idx++].set(scaledRect.left(), scaledRect.top() + radius);
    
    // Top-left corner
    for (int i = 0; i <= segments; ++i) {
        qreal angle = M_PI + (M_PI_2) * i / segments;
        qreal x = scaledRect.left() + radius + radius * cos(angle);
        qreal y = scaledRect.top() + radius + radius * sin(angle);
        vertices[idx++].set(x, y);
    }

    // Top-right corner
    for (int i = 0; i <= segments; ++i) {
        qreal angle = -M_PI_2 + (M_PI_2) * i / segments;
        qreal x = scaledRect.right() - radius + radius * cos(angle);
        qreal y = scaledRect.top() + radius + radius * sin(angle);
        vertices[idx++].set(x, y);
    }

    // Bottom-right corner
    for (int i = 0; i <= segments; ++i) {
        qreal angle = (M_PI_2) * i / segments;
        qreal x = scaledRect.right() - radius + radius * cos(angle);
        qreal y = scaledRect.bottom() - radius + radius * sin(angle);
        vertices[idx++].set(x, y);
    }

    // Bottom-left corner
    for (int i = 0; i <= segments; ++i) {
        qreal angle = M_PI_2 + (M_PI_2) * i / segments;
        qreal x = scaledRect.left() + radius + radius * cos(angle);
        qreal y = scaledRect.bottom() - radius + radius * sin(angle);
        vertices[idx++].set(x, y);
    }

    // Explicitly close path by returning to start point
    vertices[idx++].set(scaledRect.left(), scaledRect.top() + radius);
    // Add final vertex at start to ensure proper closure
    vertices[idx].set(scaledRect.left(), scaledRect.top() + radius);
    
    borderNode->setGeometry(borderGeometry);
    borderNode->setFlag(QSGNode::OwnsGeometry);
    
    QSGFlatColorMaterial *borderMaterial = new QSGFlatColorMaterial;
    borderMaterial->setColor(QColor(255, 255, 255, 230));
    
    borderNode->setMaterial(borderMaterial);
    borderNode->setFlag(QSGNode::OwnsMaterial);
    
    container->appendChildNode(borderNode);
}

// Helper method for title overlay
void CustomImageListView::addTitleOverlay(QSGNode* container, const QRectF& rect, const QString& title)
{
    QRectF textRect(rect.left(), rect.bottom() - 40, rect.width(), 40);
    
    // Add semi-transparent background
    QSGGeometryNode *textBgNode = new QSGGeometryNode;
    QSGGeometry *bgGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4);
    bgGeometry->setDrawingMode(GL_TRIANGLE_STRIP);
    
    QSGGeometry::Point2D *bgVertices = bgGeometry->vertexDataAsPoint2D();
    bgVertices[0].set(textRect.left(), textRect.top());
    bgVertices[1].set(textRect.right(), textRect.top());
    bgVertices[2].set(textRect.left(), textRect.bottom());
    bgVertices[3].set(textRect.right(), textRect.bottom());
    
    textBgNode->setGeometry(bgGeometry);
    textBgNode->setFlag(QSGNode::OwnsGeometry);
    
    QSGFlatColorMaterial *bgMaterial = new QSGFlatColorMaterial;
    bgMaterial->setColor(QColor(0, 0, 0, 128));
    textBgNode->setMaterial(bgMaterial);
    textBgNode->setFlag(QSGNode::OwnsMaterial);
    
    container->appendChildNode(textBgNode);

    // Add text
    QSGGeometryNode *textNode = createOptimizedTextNode(title, textRect);
    if (textNode) {
        container->appendChildNode(textNode);
    }
}

// Add this new function
void CustomImageListView::debugResourceSystem() const 
{
    qDebug() << "\n=== Resource System Debug ===";
    qDebug() << "Application dir:" << QCoreApplication::applicationDirPath();
    qDebug() << "Current working directory:" << QDir::currentPath();
    
    // Check resource root
    QDir resourceRoot(":/");
    qDebug() << "\nResource root contents:";
    for(const QString &entry : resourceRoot.entryList()) {
        qDebug() << " -" << entry;
    }
    
    // Check images directory
    QDir imagesDir(":/images");
    qDebug() << "\nImages directory contents:";
    for(const QString &entry : imagesDir.entryList()) {
        qDebug() << " -" << entry;
    }
    
    // Try to open each image path variation
    QString testPath = "/images/img1.jpg";
    QStringList pathsToTest;
    pathsToTest << testPath
                << ":" + testPath
                << "qrc:" + testPath
                << QCoreApplication::applicationDirPath() + testPath;
    
    qDebug() << "\nTesting image paths:";
    for(const QString &path : pathsToTest) {
        qDebug() << "\nTesting path:" << path;
        QFile file(path);
        if (file.exists()) {
            qDebug() << " - File exists";
            if (file.open(QIODevice::ReadOnly)) {
                qDebug() << " - Can open file";
                qDebug() << " - File size:" << file.size();
                file.close();
            } else {
                qDebug() << " - Cannot open file:" << file.errorString();
            }
        } else {
            qDebug() << " - File does not exist";
        }
    }
}

void CustomImageListView::itemChange(ItemChange change, const ItemChangeData &data)
{
    // If we're removed from scene, mark destruction and clean up
    if (change == ItemSceneChange) {
        if (data.window == nullptr) {
            // Item being removed from scene
            m_isBeingDestroyed = true;
            
            // Block rendering before cleanup
            setVisible(false);
            
            // Ensure nodes won't be accessed
            QQuickWindow* oldWindow = window();
            if (oldWindow) {
                disconnect(oldWindow, SIGNAL(beforeRendering()), this, nullptr);
                disconnect(oldWindow, SIGNAL(afterRendering()), this, nullptr);
                
                // Force update to apply visibility change
                oldWindow->update();
            }
            
            // Use QPointer to safely track object lifetime
            QPointer<CustomImageListView> safeThis = this;
            QTimer::singleShot(0, [safeThis]() {
                // Only call safeCleanup if the object still exists
                if (safeThis) {
                    safeThis->safeCleanup();
                }
            });
        }
    }
    QQuickItem::itemChange(change, data);
}

void CustomImageListView::tryLoadImages()
{
    if (!m_windowReady || !window()) {
        qDebug() << "Window not ready, deferring image loading";
        return;
    }
    
    if (width() <= 0 || height() <= 0) {
        qDebug() << "Invalid dimensions, deferring image loading";
        return;
    }

    loadAllImages();
}

void CustomImageListView::setCurrentIndex(int index)
{
    if (m_currentIndex != index && index >= 0 && index < m_count) {
        m_currentIndex = index;
        updateCurrentCategory();
        
        // Emit full JSON data when focus changes
        if (index < m_imageData.size()) {
            const ImageData &currentItem = m_imageData[index];
            
            // Find original JSON object
            QJsonArray items = m_parsedJson["menuItems"].toObject()["items"].toArray();
            for (const QJsonValue &rowVal : items) {
                QJsonArray rowItems = rowVal.toObject()["items"].toArray();
                for (const QJsonValue &itemVal : rowItems) {
                    QJsonObject item = itemVal.toObject();
                    if (item["title"].toString() == currentItem.title) {
                        emit assetFocused(item);
                        break;
                    }
                }
            }
        }
        
        emit currentIndexChanged();
        update();
    }
}

void CustomImageListView::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Key pressed:" << event->key() << "Has focus:" << hasActiveFocus();
    
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Space:
            handleKeyAction(Qt::Key_Return);
            event->accept();
            break;
        case Qt::Key_I:
            handleKeyAction(Qt::Key_I);
            event->accept();
            break;
        case Qt::Key_Left:        // Add back left navigation
            navigateLeft();
            event->accept();
            break;
        case Qt::Key_Right:       // Add back right navigation
            navigateRight();
            event->accept();
            break;
        case Qt::Key_Up:
            navigateUp();
            event->accept();
            break;
        case Qt::Key_Down:
            navigateDown();
            event->accept();
            break;
        default:
            QQuickItem::keyPressEvent(event);
    }
}

void CustomImageListView::handleKeyAction(Qt::Key key)
{
    if (m_currentIndex < 0 || m_currentIndex >= m_imageData.size()) {
        return;
    }

    const ImageData &currentItem = m_imageData[m_currentIndex];
    
    // Find the complete JSON object for current item
    QJsonArray rows = m_parsedJson["menuItems"].toObject()["items"].toArray();
    for (const QJsonValue &rowVal : rows) {
        QJsonArray items = rowVal.toObject()["items"].toArray();
        for (const QJsonValue &itemVal : items) {
            QJsonObject item = itemVal.toObject();
            if (item["title"].toString() == currentItem.title) {
                // Found matching item, now handle the action
                if (key == Qt::Key_Return || key == Qt::Key_Space) {
                    QJsonObject actionData;
                    actionData["action"] = "OK";
                    actionData["title"] = item["title"].toString();
                    actionData["description"] = item["shortSynopsis"].toString();
                    actionData["category"] = currentItem.category;
                    actionData["id"] = item["assetType"].toString();
                    actionData["thumbnailUrl"] = item["thumbnailUri"].toString();
                    actionData["moodImageUri"] = item["moodImageUri"].toString();
                    
                    // Add complete links array from original JSON
                    actionData["links"] = item["links"];
                    
                    qDebug() << "Emitting action data:" << actionData;
                    emit linkActivated("OK", QJsonDocument(actionData).toJson(QJsonDocument::Compact));
                }
                else if (key == Qt::Key_I) {
                    QJsonObject actionData;
                    actionData["action"] = "info";
                    actionData["title"] = item["title"].toString();
                    actionData["description"] = item["shortSynopsis"].toString();
                    actionData["category"] = currentItem.category;
                    actionData["id"] = item["assetType"].toString();
                    actionData["thumbnailUrl"] = item["thumbnailUri"].toString();
                    actionData["moodImageUri"] = item["moodImageUri"].toString();
                    
                    // Add complete links array from original JSON
                    actionData["links"] = item["links"];
                    
                    qDebug() << "Emitting action data:" << actionData;
                    emit linkActivated("info", QJsonDocument(actionData).toJson(QJsonDocument::Compact));
                }
                break;
            }
        }
    }
}

bool CustomImageListView::navigateLeft()
{
    if (m_currentIndex < 0 || m_imageData.isEmpty()) {
        return false;
    }

    QString currentCategory = m_imageData[m_currentIndex].category;
    int prevIndex = m_currentIndex - 1;
    
    // Check if we're at the leftmost item in the category
    bool isLeftmost = true;
    for (int i = 0; i < m_currentIndex; i++) {
        if (m_imageData[i].category == currentCategory) {
            isLeftmost = false;
            break;
        }
    }
    
    if (isLeftmost) {
        qDebug() << "At leftmost position, letting event propagate";
        return false;  // This will cause the event to propagate up
    }
    
    // Handle navigation within the category
    while (prevIndex >= 0) {
        if (m_imageData[prevIndex].category != currentCategory) {
            break;
        }
        
        // Get category dimensions for animation calculations
        CategoryDimensions dims = getDimensionsForCategory(currentCategory);
        
        setCurrentIndex(prevIndex);
        ensureFocus();
        updateCurrentCategory();
        
        // Calculate scroll target position with animation
        int itemsBeforeInCategory = 0;
        for (int i = 0; i < prevIndex; i++) {
            if (m_imageData[i].category == currentCategory) {
                itemsBeforeInCategory++;
            }
        }
        
        qreal targetX = itemsBeforeInCategory * (dims.posterWidth + dims.itemSpacing);
        targetX = qMax(0.0, targetX - (width() - dims.posterWidth) / 2);
        
        // Animate to target position
        animateScroll(currentCategory, targetX);
        
        ensureIndexVisible(prevIndex);
        update();
        
        // Add cleanup check after scrolling
        if (m_nodes.size() > CLEANUP_THRESHOLD) {
            recycleOffscreenTextures();
        }
        
        return true;
    }
    
    return false;
}

void CustomImageListView::navigateRight()
{
    QString currentCategory = m_imageData[m_currentIndex].category;
    int nextIndex = m_currentIndex + 1;
    
    while (nextIndex < m_imageData.size()) {
        if (m_imageData[nextIndex].category != currentCategory) {
            break;
        }
        
        // Get category dimensions and calculate if scrolling is needed
        CategoryDimensions dims = getDimensionsForCategory(currentCategory);
        int itemsInCategory = 0;
        int lastIndexInCategory = nextIndex;
        
        // Count items in this category and find last index
        for (int i = 0; i < m_imageData.size(); i++) {
            if (m_imageData[i].category == currentCategory) {
                itemsInCategory++;
                lastIndexInCategory = i;
            }
        }
        
        setCurrentIndex(nextIndex);
        ensureIndexVisible(nextIndex);
        ensureFocus();
        updateCurrentCategory();
        
        // Calculate scroll target position with animation
        qreal totalWidth = itemsInCategory * dims.posterWidth + 
                          (itemsInCategory - 1) * dims.itemSpacing;
                          
        if (totalWidth > width()) {
            qreal targetX;
            if (nextIndex == lastIndexInCategory) {
                targetX = categoryContentWidth(currentCategory) - width();
            } else {
                int itemsBeforeInCategory = 0;
                for (int i = 0; i < nextIndex; i++) {
                    if (m_imageData[i].category == currentCategory) {
                        itemsBeforeInCategory++;
                    }
                }
                targetX = itemsBeforeInCategory * (dims.posterWidth + dims.itemSpacing);
                targetX = qMax(0.0, targetX - (width() - dims.posterWidth) / 2);
            }
            
            // Animate to target position
            animateScroll(currentCategory, targetX);
        }
        
        update();
        
        // Add cleanup check after scrolling
        if (m_nodes.size() > CLEANUP_THRESHOLD) {
            recycleOffscreenTextures();
        }
        
        return;
    }
}

// Update navigateUp and navigateDown methods
void CustomImageListView::navigateUp()
{
    if (m_currentIndex < 0 || m_rowTitles.isEmpty()) {
        return;
    }

    QString currentCategory = m_imageData[m_currentIndex].category;
    int categoryIndex = m_rowTitles.indexOf(currentCategory);
    
    if (categoryIndex <= 0) {
        return;
    }
    
    QString prevCategory = m_rowTitles[categoryIndex - 1];
    
    int targetIndex = findMatchingPositionInNextCategory(currentCategory, prevCategory);
    
    if (targetIndex != -1) {
        // Calculate target position ensuring item remains visible
        qreal targetY = calculateItemVerticalPosition(targetIndex);
        CategoryDimensions dims = getDimensionsForCategory(prevCategory);
        
        // Calculate proper scroll position that centers the item
        qreal viewportCenter = height() / 2;
        qreal desiredY = targetY - viewportCenter + (dims.rowHeight / 2);
        
        // Bound the scroll position
        qreal maxScroll = contentHeight() - height();
        desiredY = qBound(0.0, desiredY, maxScroll);
        
        // Animate to new position
        QPropertyAnimation* anim = new QPropertyAnimation(this, "contentY");
        anim->setDuration(300);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setStartValue(m_contentY);
        anim->setEndValue(desiredY);
        
        // Update selection after animation
        connect(anim, &QPropertyAnimation::finished, this, [this, targetIndex]() {
            setCurrentIndex(targetIndex);
            ensureFocus();
            ensureIndexVisible(targetIndex);
            update();
        });
        
        connect(anim, &QPropertyAnimation::finished, anim, &QPropertyAnimation::deleteLater);
        anim->start();
        
        // Preload textures for target category
        m_visibleCategories.insert(prevCategory);
        QTimer::singleShot(0, this, [this, prevCategory]() {
            preloadRowTextures(prevCategory);
        });
    }
}

// Fix #3: Improved last row visibility
void CustomImageListView::navigateDown()
{
    if (m_currentIndex < 0 || m_rowTitles.isEmpty()) {
        return;
    }

    QString currentCategory = m_imageData[m_currentIndex].category;
    int categoryIndex = m_rowTitles.indexOf(currentCategory);
    
    if (categoryIndex >= m_rowTitles.size() - 1) {
        return;
    }
    
    QString nextCategory = m_rowTitles[categoryIndex + 1];
    int targetIndex = findMatchingPositionInNextCategory(currentCategory, nextCategory);
    
    if (targetIndex != -1) {
        CategoryDimensions dims = getDimensionsForCategory(nextCategory);
        bool isLastCategory = (categoryIndex == m_rowTitles.size() - 2);
        
        // Calculate vertical positions
        qreal targetY = calculateItemVerticalPosition(targetIndex);
        qreal viewportHeight = height();
        qreal desiredY;

        if (isLastCategory) {
            // For the last category, special handling to ensure it's fully visible
            // Calculate the position of the item in the last row
            qreal itemY = targetY;
            
            // Calculate the position that would show the entire row including focus effects
            qreal rowBottom = itemY + dims.posterHeight + (dims.posterHeight * 0.2); // Add 20% for focus
            
            // Position so the row bottom is at 90% of the viewport (leaves room for effects)
            desiredY = rowBottom - (viewportHeight * 0.9);
            
            // Never scroll past content maximum
            qreal maxScroll = contentHeight() - viewportHeight;
            desiredY = qMin(desiredY, maxScroll);
        } else {
            // For non-last categories, center the row in the viewport
            desiredY = targetY - (viewportHeight - dims.rowHeight) / 2;
            
            // Apply standard bounds
            qreal maxScroll = contentHeight() - viewportHeight;
            desiredY = qBound(0.0, desiredY, maxScroll);
        }
        
        // Create animation with longer duration for smoother effect
        QPropertyAnimation* anim = new QPropertyAnimation(this, "contentY");
        anim->setDuration(350); // Slightly longer duration for smoother scrolling
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setStartValue(m_contentY);
        anim->setEndValue(desiredY);
        
        // Update selection and load textures after animation
        connect(anim, &QPropertyAnimation::finished, this, [this, targetIndex, nextCategory, isLastCategory]() {
            setCurrentIndex(targetIndex);
            ensureFocus();
            
            // Force immediate texture load for visible items with larger buffer
            m_visibleCategories.insert(nextCategory);
            loadVisibleTextures();
            
            // Additional check specifically for last row visibility
            if (isLastCategory) {
                // Short delay to ensure UI has updated
                QTimer::singleShot(20, this, [this]() {
                    ensureLastRowFullyVisible();
                });
            }
            
            update();
        });
        
        connect(anim, &QPropertyAnimation::finished, anim, &QPropertyAnimation::deleteLater);
        anim->start();
        
        // Start preloading textures immediately with more buffer
        m_visibleCategories.insert(nextCategory);
        preloadRowTextures(nextCategory);
    }
}

// Completely redesign this function for reliable last row visibility
void CustomImageListView::ensureLastRowFullyVisible()
{
    if (m_currentIndex < 0 || m_imageData.isEmpty()) {
        return;
    }

    QString category = m_imageData[m_currentIndex].category;
    int categoryIndex = m_rowTitles.indexOf(category);
    
    // Only apply special handling for the last category
    if (categoryIndex != m_rowTitles.size() - 1) {
        return;
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
