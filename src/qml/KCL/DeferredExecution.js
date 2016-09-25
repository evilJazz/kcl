/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

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
    var timerQml = "import QtQuick 2.0; " + (useQtTimers ? "import KCL 1.0; QtTimer {" : "Timer {");
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
