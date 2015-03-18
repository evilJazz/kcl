var scheduledTimers = {};

var debug = false;

// QtTimers will trigger everytime a Qt event loop is running.
// QML Timers are strictly serialized and scheduled by the declarative engine.
// Another Timer can't trigger as long as the execution of another Timer has not finished.
// You will run into trouble when there is a separate event loop (modal window, drag & drop etc.) started from within a Timer.
// Use QtTimers in this case, they work recursively.
var useQtTimers = true;

function isScheduled(name)
{
    return scheduledTimers.hasOwnProperty(name);
}

function createNewTimer(parent)
{
    var timerQml = "import QtQuick 1.0; " + (useQtTimers ? "import KCL 1.0; QtTimer {" : "Timer {");
    timerQml += (debug ? "Component.onDestruction: console.log('NUCULAR!!!'); }" : "}");

    return Qt.createQmlObject(timerQml, parent, "dynamicTimer");
}

function stop(name)
{
    if (isScheduled(name))
    {
        if (debug) console.log("Stopping scheduled timer: " + name);
        scheduledTimers[name].stop();
        scheduledTimers[name].destroy();
        delete scheduledTimers[name];
    }
    else
    {
        if (debug) console.log("Scheduled timer " + name + " does not exist.");
    }
}

function invoke(func, name, parent)
{
    executeIn(1, func, name, parent);
}

function executeIn(ms, func, name, parent)
{
    if (debug) console.log("Starting timer " + name + " in " + ms + " ms.");

    var namedTimer = typeof(name) != "undefined";

    var timerParent = typeof(parent) != "undefined" ? parent : (Qt.hasOwnProperty("application") ? Qt.application : app); // Use Qt.application on QtQuick 1.1 and up, else fall back to root item id "app".
    var timer = createNewTimer(timerParent);

    var triggerFunc = function()
    {
        if (debug) console.log("Firing timer " + name);

        try
        {
            func();
        }
        finally
        {
            if (namedTimer)
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

    if (namedTimer)
    {
        stop(name);
        scheduledTimers[name] = timer;
    }

    timer.start();
    if (debug) console.log("Timer started");
}
