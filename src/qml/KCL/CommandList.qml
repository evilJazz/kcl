import QtQuick 1.0

Item {

    property variant commands: [{}]

    property ListModel model: ListModel {
        ListElement { name: ""; value: "" }
    }

    function executeCommand(commandName, commandParam)
    {
        commands[commandName].execute(commandParam);
    }

    function registerAllCommands()
    {
        model.clear();

        var newCommands = {};

        for (var i = 0; i < data.length; ++i)
        {
            var currentCommand = data[i];

            if (quasarCore.isDebugBuild) console.log("Register command " + currentCommand);

            if (!currentCommand.hasOwnProperty("command"))
            {
                if (quasarCore.isDebugBuild) console.log("Command " + currentCommand + " is missing the command property.");
                continue;
            }

            if (!currentCommand.hasOwnProperty("text"))
            {
                if (quasarCore.isDebugBuild) console.log("Command " + currentCommand + " is missing the text property.");
                continue;
            }

            if (!currentCommand.hasOwnProperty("execute"))
            {
                if (quasarCore.isDebugBuild) console.log("Command " + currentCommand + " is missing the execute function.");
                continue;
            }

            if (newCommands.hasOwnProperty(currentCommand.command))
            {
                if (quasarCore.isDebugBuild) console.log("Already contains a command by the name of " + currentCommand.command);
                continue;
            }

            newCommands[currentCommand.command] = currentCommand;
            model.append({ name: currentCommand.text, value: currentCommand.command });
        }

        commands = newCommands;
    }

    Component.onCompleted: registerAllCommands()
}
