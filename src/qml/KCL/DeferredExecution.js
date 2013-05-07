var scheduledTimers = {};

var debug = false;

if (debug)
    var timerQml = "import QtQuick 1.0; Timer { Component.onDestruction: console.log('NUCULAR!!!'); }";
else
    var timerQml = "import QtQuick 1.0; Timer {}";

function isScheduled(name)
{
    return scheduledTimers.hasOwnProperty(name);
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

function executeIn(ms, func, name)
{
    if (debug) console.log("Starting timer " + name + " in " + ms + " ms.");

    var timer = Qt.createQmlObject(timerQml, app, "dynamicTimer");
    timer.repeat = false;
    timer.interval = ms;
    timer.triggered.connect(function()
    {
        if (debug) console.log("Firing timer " + name);
        func();

        if (typeof(name) != "undefined")
            stop(name);
        else
            timer.destroy();

        if (debug) console.log("Done firing.");
    });

    if (typeof(name) != "undefined")
    {
        stop(name);
        scheduledTimers[name] = timer;
    }

    timer.start();
    if (debug) console.log("Timer started");
}
