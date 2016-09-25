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

import QtQuick 2.0
import "QueueManager.js" as Internal

Item {
    id: queueManager

    property bool debug: false

    property int subjectsCount: 0
    property int resultsCount: 0

    property int currentIndex: -1

    property int workerActive: 0
    property int workerCount: 8

    property bool running: workerActive > 0

    function result(index)
    {
        return Internal.workResults[index]
    }

    function results()
    {
        return Internal.workResults;
    }

    function subject(index)
    {
        return Internal.workSubjects[index];
    }

    function subjects()
    {
        return Internal.workSubjects;
    }

    signal startWorkerOnSubject(int index, variant currentSubject)
    signal finished()

    function enqueueMany(newSubjects)
    {
        Internal.workSubjects = Internal.workSubjects.concat(newSubjects);
        subjectsCount = Internal.workSubjects.length;
    }

    function enqueueOne(newSubject)
    {
        Internal.workSubjects.push(newSubject);
        subjectsCount = Internal.workSubjects.length;
    }

    function reset()
    {
        Internal.workResults = [];
        Internal.workSubjects = [];
        currentIndex = -1;
        subjectsCount = 0;
        resultsCount = 0;
        workerActive = 0;
    }

    function resetResults()
    {
        Internal.workResults = [];
        resultsCount = 0;
    }

    function startWorkers()
    {
        if (currentIndex < Internal.workSubjects.length - 1)
        {
            while ((workerActive < workerCount) && (currentIndex < Internal.workSubjects.length - 1))
                workOnNextSubject();
        }
        else // if (currentIndex == subjects.length - 1)
        {
            // nothing to do anymore...
            if (workerActive === 0)
            {
                if (debug) console.log("Finished...");
                resultsCount = Internal.workResults.length;
                finished();
            }
        }
    }

    function workerFinished(index, result)
    {
        if (debug) console.log("workerFinished(" + index + ", " + result + "): workerActive: " + workerActive);
        --workerActive;

        Internal.workResults[index] = result;
        resultsCount = Internal.workResults.length;

        if (debug) console.log("Restarting timer...");
        nextResultTimer.restart();
    }

    function workOnNextSubject()
    {
        if (debug) console.log("workOnNextSubject(): workerActive: " + workerActive);
        if (workerActive == workerCount)
        {
            if (debug) console.log("All fetchers are working...");
            return;
        }

        ++workerActive;
        ++currentIndex;
        startWorkerOnSubject(currentIndex, Internal.workSubjects[currentIndex]);
    }

    Timer {
        id: nextResultTimer
        interval: 1
        repeat: false
        onTriggered: startWorkers()
    }
}
