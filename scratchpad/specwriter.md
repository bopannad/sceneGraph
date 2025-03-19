# Qt Scenegraph Movie Browser Widget Specification

## Overview

A high-performance, low-memory Netflix-style home screen widget built with Qt 5.6 Scenegraph optimized for Raspberry Pi. The widget displays movie thumbnails in horizontally scrollable rows with a TV remote-style navigation system.

## Functional Specification

### Core Features

1. **Content Layout**
   - Multiple horizontally scrolling rows of content thumbnails
   - Vertical navigation between rows
   - Maximum 4 rows visible/loaded at any time
   - Maximum 8 thumbnails visible per row with 2-3 additional thumbnails cached for smooth scrolling

2. **Navigation System**
   - TV remote-style control using directional keys (up, down, left, right) and OK button
   - Smooth scrolling animations when navigating between items
   - Visual focus indicator for the currently selected item (1.2x zoom)

3. **Content Display**
   - Movie thumbnails displayed from cloud API data
   - JSON data structure handling for movie metadata

4. **Performance Features**
   - Optimized image loading and unloading
   - Efficient memory management suitable for embedded systems
   - Smooth scrolling and animations even on limited hardware

### User Interaction Flow

1. Widget initializes and loads initial content (first 4 rows)
2. User navigates horizontally within a row using left/right keys
   - Selected thumbnail enlarges to 1.2x normal size
   - Smooth scrolling animation when moving between items
   - When approaching edge of visible items, new thumbnails load while old ones unload
3. User navigates vertically between rows using up/down keys
   - Focus moves to the nearest item in the adjacent row
   - When navigating to rows at the edge, new rows load while old ones unload
4. User selects an item using the OK button
   - Application triggers the item selection callback

## Technical Specification

### Architecture

The widget uses a layered architecture:

1. **Data Layer**
   - Handles JSON API communication
   - Parses movie data
   - Manages image loading and caching

2. **Scene Graph Layer**
   - Main QQuickItem-based widget
   - Custom QSGNode implementations for optimal rendering
   - Texture caching and management

3. **Control Layer**
   - Input handling (key events)
   - Focus management
   - Animation control

### Key Components

1. **MovieBrowserWidget** (Main QQuickItem)
   - Entry point for the widget
   - Manages overall geometry and layout
   - Handles key events and dispatches to appropriate handlers

2. **ContentRowManager**
   - Manages the currently visible rows
   - Handles row creation, destruction, and recycling
   - Implements vertical scrolling logic

3. **ContentRow**
   - Manages individual horizontal rows
   - Handles item creation, destruction, and recycling
   - Implements horizontal scrolling logic

4. **ThumbnailItem**
   - Represents individual movie thumbnails
   - Manages thumbnail image loading and rendering
   - Implements zoom animation for selection

5. **ImageCache**
   - Manages efficient loading and unloading of images
   - Implements LRU (Least Recently Used) caching strategy
   - Optimizes memory usage

6. **ApiDataProvider**
   - Handles communication with cloud API
   - Parses JSON responses
   - Provides data to content rows

### Optimization Techniques

1. **Texture Atlasing**
   - Group multiple thumbnail textures into single GPU textures where appropriate
   - Reduce texture binding operations

2. **Node Pooling**
   - Reuse scene graph nodes instead of creating/destroying
   - Minimize memory allocation/deallocation overhead

3. **Viewport-Based Loading**
   - Only load images for visible and near-visible items
   - Unload images for items far from the viewport

4. **Asynchronous Image Loading**
   - Load images in background threads to maintain UI responsiveness
   - Prioritize loading of visible/soon-to-be-visible items

5. **Item Recycling**
   - Reuse row and thumbnail components when scrolling
   - Maintain fixed memory footprint regardless of content size

### Qt Technologies Used

1. **Qt Quick Scenegraph**
   - Custom QSGNode implementations for optimal rendering
   - Direct OpenGL rendering via scenegraph for maximum performance

2. **Qt Network**
   - For API communication and image downloading

3. **Qt QML/C++ Integration**
   - C++ implementation for performance-critical components
   - QML for declarative UI aspects if needed

### Integration Specification

1. **Cloud API Integration**
   - Expects JSON data with movie metadata including:
     - Thumbnail image URLs
     - Movie titles
     - Row categorization information
   - RESTful API communication

2. **Widget Integration**
   - Will be provided as a QML importable module
   - Can be embedded in larger Qt applications
   - Exposes properties and signals for customization and event handling

3. **Image Loading**
   - Support for common image formats (JPEG, PNG)
   - Scaled loading based on display resolution
   - Image caching to minimize network requests

## Implementation Notes

1. All rendering will be optimized specifically for Qt 5.6 Scenegraph and Raspberry Pi hardware
2. Code will avoid memory allocations during scrolling operations
3. Texture memory will be strictly managed to stay within embedded system constraints
4. All animations will be implemented using the scenegraph for optimal performance
5. Widget will implement efficient focus handling for TV remote navigation