QMLPP_REWRITEVERSION =
QMLPP_DEFINES =
QMLPP_FOLDERS =
QMLPP_PATH = $${PWD}
QMLPP_SCRIPT = $${QMLPP_PATH}/qmlpp.sh

# qmlPreprocessFolder(folders, defines, rewriteVersion)
defineTest(qmlPreprocessFolder) {
    !isEmpty(1) {
        qmlppCommand = $${QMLPP_SCRIPT} -i
        qmlppCommands =

        !isEmpty(3): qmlppCommand += -q $${3}
        !isEmpty(2) {
            defines = $$join(2, "|")
            qmlppCommand += -d \"$$defines\"
        }

        for(folder, 1) {
            message(folder: $$folder)
            !isEmpty(qmlppCommands): qmlppCommands += &&
            qmlppCommands += $${qmlppCommand} \"$$folder\"
        }

        system($$qmlppCommands)
    }
}

defineTest(qmlPreprocessFolders) {
    qmlPreprocessFolder($$QMLPP_FOLDERS, $$QMLPP_DEFINES, $$QMLPP_REWRITEVERSION)
}

#qmlPreprocessFolder(test blah qml, @QtQuick2 @QtQuick1 @QtQuick3, 2.1)
#qmlPreprocessFolders()
