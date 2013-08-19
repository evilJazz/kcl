var scheduledTimers = {};

var debug = false;

// QtTimers will trigger everytime a Qt event loop is running.
// QML Timers are strictly serialized and scheduled by the declarative engine.
// Another Timer can't trigger as long as the execution of another Timer has not finished.
// You will run into trouble when there is a separate event loop (modal window, drag & drop etc.) started from within a Timer. Use QtTimers in this case.
var useQtTimers = true;

function isScheduled(name)
{
    return scheduledTimers.hasOwnProperty(name);
}

function createNewTimer(parent)
{
    var timerQml = "import QtQuick 1.0; " + useQtTimers ? "import KCL 1.0; QtTimer {" : "Timer {";

    timerQml += !useQtTimers && debug ? "Component.onDestruction: console.log('NUCULAR!!!'); }" : "}";

    return Qt.createQmlObject(timerQml, parent, "dynamicTimer");
}

function stop(name)
{
    if (isScheduled(name))
    {
        if (debug) console.log("Stopping scheduled timer: " + name);
        scheduledTimers[name].destroy();
        delete scheduledTimers[name];
    }
    else
    {
        if (debug) console.log("Scheduled timer " + name + " does not exist.");
    }
}

function invoke(func, name)
{
    executeIn(1, func, name);
}

function executeIn(ms, func, name)
{
    if (debug) console.log("Starting timer " + name + " in " + ms + " ms.");

    var timer = createNewTimer(app);

    var triggerFunc = function()
    {
        if (debug) console.log("Firing timer " + name);

        try
        {
            func();
        }
        finally
        {
            if (typeof(name) != "undefined")
                stop(name);
            else
                timer.destroy();

            if (debug) console.log("Done firing.");
        }
    };

    timer.interval = ms;

    if (useQtTimers)
    {
        timer.singleShot = true;
        timer.timeout.connect(triggerFunc);
    }
    else
    {
        timer.repeat = false;
        timer.triggered.connect(triggerFunc);
    }

    if (typeof(name) != "undefined")
    {
        stop(name);
        scheduledTimers[name] = timer;
    }

    timer.start();
    if (debug) console.log("Timer started");
}
