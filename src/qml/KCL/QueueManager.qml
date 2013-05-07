import QtQuick 1.0
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
