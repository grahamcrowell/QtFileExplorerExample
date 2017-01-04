/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QFileSystemModel>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <iostream>

static inline QString permissionString(const QFileInfo &fi)
{
    const QFile::Permissions permissions = fi.permissions();
    QString result = QLatin1String("----------");
    if (fi.isSymLink())
        result[0] = QLatin1Char('l');
    else if (fi.isDir())
        result[0] = QLatin1Char('d');
    if (permissions & QFileDevice::ReadUser)
        result[1] = QLatin1Char('r');
    if (permissions & QFileDevice::WriteUser)
        result[2] = QLatin1Char('w');
    if (permissions & QFileDevice::ExeUser)
        result[3] = QLatin1Char('x');
    if (permissions & QFileDevice::ReadGroup)
        result[4] = QLatin1Char('r');
    if (permissions & QFileDevice::WriteGroup)
        result[5] = QLatin1Char('w');
    if (permissions & QFileDevice::ExeGroup)
        result[6] = QLatin1Char('x');
    if (permissions & QFileDevice::ReadOther)
        result[7] = QLatin1Char('r');
    if (permissions & QFileDevice::WriteOther)
        result[8] = QLatin1Char('w');
    if (permissions & QFileDevice::ExeOther)
        result[9] = QLatin1Char('x');
    return result;
}

static inline QString sizeString(const QFileInfo &fi)
{
    if (!fi.isFile())
        return QString();
    const qint64 size = fi.size();
    if (size > 1024 * 1024 * 10)
        return QString::number(size / (1024 * 1024)) + QLatin1Char('M');
    if (size > 1024 * 10)
        return QString::number(size / 1024) + QLatin1Char('K');
    return QString::number(size);
}

class DisplayFileSystemModel : public QFileSystemModel {
    Q_OBJECT
public:
    explicit DisplayFileSystemModel(QObject *parent = Q_NULLPTR)
        : QFileSystemModel(parent) {}

    // these are columns describing file/folder
    enum Roles  {
        SizeRole = Qt::UserRole + 4,
        DisplayableFilePermissionsRole = Qt::UserRole + 5,
        LastModifiedRole = Qt::UserRole + 6,
        UrlStringRole = Qt::UserRole + 7
    };
    Q_ENUM(Roles)

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        std::cout << "QModelIndex row = " << index.row() << " column " << index.column() << std::endl;
        std::cout << "Qt::DisplayRole = " << role << std::endl; // ~ 258 - 262
        std::string tmp_data {};
        if (index.isValid() && role >= SizeRole) {

            switch (role) {
            case SizeRole:
                tmp_data = std::string(sizeString(fileInfo(index)).toStdString());
                std::cout << "model.data [SizeRole] = " << tmp_data << std::endl;
                return QVariant(QString::fromStdString(tmp_data));
            case DisplayableFilePermissionsRole:
                tmp_data = std::string(permissionString(fileInfo(index)).toStdString());
                std::cout << "model.data [DisplayableFilePermissionsRole] = " << tmp_data << std::endl;
                return QVariant(QString::fromStdString(tmp_data));
            case LastModifiedRole:
                tmp_data = std::string(fileInfo(index).lastModified().toString(Qt::SystemLocaleShortDate).toStdString());
                std::cout << "model.data [LastModifiedRole] = " << tmp_data << std::endl;
                return QVariant(QString::fromStdString(tmp_data));
            case UrlStringRole:
                tmp_data = std::string(QUrl::fromLocalFile(filePath(index)).toString().toStdString());
                std::cout << "model.data [UrlStringRole] = " << tmp_data << std::endl;
                return QVariant(QString::fromStdString(tmp_data));
            default:
                break;
            }
        }
        QVariant qvar(QFileSystemModel::data(index, role));
        QString qstring(qvar.toString());
        tmp_data = qstring.toStdString();
        std::cout << "model.data [name?] = " << tmp_data << std::endl;
        return qvar;
    }

    // role
    // return roleNames in dictionary container (key=int, value=string literal)
    // key is enum offset by Qt::UserRole
    // value is used for role property in qml
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE
    {
        QHash<int, QByteArray> result = QFileSystemModel::roleNames();
        result.insert(SizeRole, QByteArrayLiteral("size"));
        result.insert(DisplayableFilePermissionsRole, QByteArrayLiteral("displayableFilePermissions"));
        result.insert(LastModifiedRole, QByteArrayLiteral("lastModified"));
        return result;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<DisplayFileSystemModel>("io.qt.examples.quick.controls.filesystembrowser", 1, 0,
                                                       "FileSystemModel", "Cannot create a FileSystemModel instance.");
    QFileSystemModel *fsm = new DisplayFileSystemModel(&engine);
    fsm->setRootPath(QDir::homePath());
    fsm->setResolveSymlinks(true);

    // filter not working
//    fsm->setNameFilters(QStringList()<<"*.exe");

    engine.rootContext()->setContextProperty("fileSystemModel", fsm);
    engine.rootContext()->setContextProperty("rootPathIndex", fsm->index(fsm->rootPath()));
    std::cout << "fsm->rootPath() = " << fsm->rootPath().toStdString() << std::endl;
    std::cout << "fsm->index(fsm->rootPath()).row() = " << fsm->index(fsm->rootPath()).row() << std::endl;
    std::cout << "fsm->index(fsm->rootPath()).column() = " << fsm->index(fsm->rootPath()).column() << std::endl;
    std::cout << "fsm->index(fsm->rootPath()).parent().isValid() = " << fsm->index(fsm->rootPath()).parent().isValid() << std::endl;

    std::cout << "file filters: " << fsm->nameFilters().size() << std::endl;
    if(fsm->nameFilters().size() > 0)
    {
        std::cout << "file filters: " << std::endl;
        std::cout << "file filters: " << fsm->nameFilters().at(0).toStdString() << std::endl;
//        std::for_each(fsm->nameFilters().begin(),fsm->nameFilters().end(),[&](QString s)
//        {
//            std::cout << s.toStdString() << std::endl;
//        });
    }
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

#include "main.moc"
