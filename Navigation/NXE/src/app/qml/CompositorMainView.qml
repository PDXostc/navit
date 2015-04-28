/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import "compositor.js" as CompositorLogic
import "nxe-ui"

Item {
    id: root
    width: 1060
    height: 1000

    property variant selectedWindow: null
    property bool allFullscreen: true

    NxeUI {
        anchors.fill: parent
        z:10
    }

    function windowAdded(window) {
        var windowContainerComponent = Qt.createComponent("WindowContainer.qml");
        if (windowContainerComponent.status !== Component.Ready) {
            console.warn("Error loading WindowContainer.qml: " +  windowContainerComponent.errorString());
            return;
        }
        var windowContainer = windowContainerComponent.createObject(root);

        windowContainer.child = compositor.item(window);
        windowContainer.child.parent = windowContainer;
        windowContainer.child.touchEventsEnabled = true;

        windowContainer.targetWidth = window.size.width;
        windowContainer.targetHeight = window.size.height;

        CompositorLogic.addWindow(windowContainer);

        console.debug('w = ' + windowContainer.width)
        windowContainer.width = root.width
        windowContainer.height = root.height

        windowContainer.opacity = 1
        windowContainer.animationsEnabled = true;
    }

    function windowResized(window) {
        window.width = window.surface.size.width;
        window.height = window.surface.size.height;

        CompositorLogic.relayout();
    }

    function removeWindow(window) {
        console.debug('remove window')
        CompositorLogic.removeWindow(window);
        window.destroy();
    }

    onHeightChanged: CompositorLogic.relayout();
    onWidthChanged: CompositorLogic.relayout();
}
