import QtQuick 2.2
import QtQuick.Controls 1.5
import QtQml.Models 2.2
//import io.qt.examples.quick.controls.filesystembrowser 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("File System")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    Row {
        id: row
        anchors.top: parent.top
        anchors.topMargin: 12
        anchors.horizontalCenter: parent.horizontalCenter

        ExclusiveGroup {
            id: eg
        }

        Repeater {
            model: [ "None", "Single", "Extended", "Multi", "Contig."]
            Button {
                text: modelData
                exclusiveGroup: eg
                checkable: true
                checked: index === 1
                onClicked: view.selectionMode = index
            }
        }
    }

    // fileSystemModel is a name set associated root context property set to a pointer to an instance of the DisplayFileSystemModel class

    ItemSelectionModel {
        id: sel
        model: fileSystemModel
    }

    TreeView {
        id: view
        anchors.fill: parent
        anchors.margins: 2 * 12 + row.height
        model: fileSystemModel
        rootIndex: rootPathIndex
        selection: sel

        TableViewColumn {
            title: "Name"
            role: "fileName"
            resizable: true
        }

        TableViewColumn {
            title: "Size"
            role: "size"
            resizable: true
            horizontalAlignment : Text.AlignRight
            width: 70
        }

        TableViewColumn {
            title: "Permissions"
            role: "displayableFilePermissions"
            resizable: true
            width: 100
        }

        TableViewColumn {
            title: "Date Modified"
            role: "lastModified"
            resizable: true
        }

        onActivated : {
//            var url = fileSystemModel.data(index, FileSystemModel.UrlStringRole)
//            Qt.openUrlExternally(url)
        }
    }
}
