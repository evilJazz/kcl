import QtQuick 2.0

Timer {
    id: countdownTimer
    interval: 500
    repeat: true

    function startCountdown(durationInMilliseconds)
    {
        startTimestamp = new Date().getTime();
        endTimestamp = startTimestamp + durationInMilliseconds;
        start();
    }

    property variant startTimestamp
    property variant endTimestamp
    property int timeLeft

    signal expired()
    signal liftOff() // Yeah! ;)

    onTriggered:
    {
        timeLeft = Math.max(0, endTimestamp - new Date().getTime());

        if (timeLeft == 0)
        {
            stop();
            countdownTimer.expired();
            countdownTimer.liftOff();
        }
    }
}
