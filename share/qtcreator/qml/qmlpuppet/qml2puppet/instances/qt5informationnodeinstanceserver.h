/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef QMLDESIGNER_QT5INFORMATIONNODEINSTANCESERVER_H
#define QMLDESIGNER_QT5INFORMATIONNODEINSTANCESERVER_H

#include "qt5nodeinstanceserver.h"
#include "tokencommand.h"

namespace QmlDesigner {

class Qt5InformationNodeInstanceServer : public Qt5NodeInstanceServer
{
    Q_OBJECT
public:
    explicit Qt5InformationNodeInstanceServer(NodeInstanceClientInterface *nodeInstanceClient);

    void reparentInstances(const ReparentInstancesCommand &command) Q_DECL_OVERRIDE;
    void clearScene(const ClearSceneCommand &command) Q_DECL_OVERRIDE;
    void createScene(const CreateSceneCommand &command) Q_DECL_OVERRIDE;
    void completeComponent(const CompleteComponentCommand &command) Q_DECL_OVERRIDE;
    void token(const TokenCommand &command) Q_DECL_OVERRIDE;
    void removeSharedMemory(const RemoveSharedMemoryCommand &command) Q_DECL_OVERRIDE;

protected:
    void collectItemChangesAndSendChangeCommands() Q_DECL_OVERRIDE;
    void sendChildrenChangedCommand(const QList<ServerNodeInstance> &childList);
    void sendTokenBack();
    bool isDirtyRecursiveForNonInstanceItems(QQuickItem *item) const;
    bool isDirtyRecursiveForParentInstances(QQuickItem *item) const;

private:
    QSet<ServerNodeInstance> m_parentChangedSet;
    QList<ServerNodeInstance> m_completedComponentList;
    QList<TokenCommand> m_tokenList;
};

} // namespace QmlDesigner

#endif // QMLDESIGNER_QT5INFORMATIONNODEINSTANCESERVER_H
