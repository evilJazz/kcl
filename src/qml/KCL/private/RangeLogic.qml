import QtQuick 2.0

QtObject {
    id: internal

    property bool debug: false

    property real minimumValue: 0.0
    onMinimumValueChanged: updatePositionFromValue()

    property real maximumValue: 1.0
    onMaximumValueChanged: updatePositionFromValue()

    property real value: 0
    onValueChanged: updatePositionFromValue()

    property real singleValueStep: 0.05
    property real pageValueStep: 0.1

    property real position: 0
    onPositionChanged: updateValueFromPosition()

    property real positionAtMinimumValue: 0
    onPositionAtMinimumValueChanged: updatePositionFromValue()

    property real positionAtMaximumValue: 100
    onPositionAtMaximumValueChanged: updatePositionFromValue()

    property bool _blockUpdates: false

    function step(diff)
    {
        if (minimumValue < maximumValue)
            value = Math.min(Math.max(value + diff, minimumValue), maximumValue);
        else
            value = Math.min(Math.max(value + diff, maximumValue), minimumValue);
    }

    function incrementValue() { step(singleValueStep); }
    function decrementValue() { step(-singleValueStep); }
    function pageIncrementValue() { step(pageValueStep); }
    function pageDecrementValue() { step(-pageValueStep); }

    function updateValueFromPosition()
    {
        if (_blockUpdates) return;

        if (position < positionAtMinimumValue || position > positionAtMaximumValue)
        {
            position = Math.min(Math.max(position, positionAtMinimumValue), positionAtMaximumValue);
            return;
        }

        _blockUpdates = true;

        if (minimumValue < maximumValue)
            value = minimumValue + Math.max(0, (maximumValue - minimumValue) * ((position - positionAtMinimumValue) / (positionAtMaximumValue - positionAtMinimumValue)));
        else
            value = maximumValue + Math.max(0, (minimumValue - maximumValue) * (1 - ((position - positionAtMinimumValue) / (positionAtMaximumValue - positionAtMinimumValue))));

        if (debug) console.log("newValue: " + value);
        _blockUpdates = false;
    }

    function updatePositionFromValue()
    {
        if (_blockUpdates) return;

        if (minimumValue < maximumValue)
        {
            if (value < minimumValue || value > maximumValue)
            {
                value = Math.min(Math.max(value, minimumValue), maximumValue);
                return;
            }
        }
        else
        {
            if (value < maximumValue || value > minimumValue)
            {
                value = Math.min(Math.max(value, maximumValue), minimumValue);
                return;
            }
        }

        _blockUpdates = true;

        if (minimumValue < maximumValue)
            position = positionAtMinimumValue + Math.max(0, (positionAtMaximumValue - positionAtMinimumValue) * ((value - minimumValue) / (maximumValue - minimumValue)));
        else
            position = positionAtMinimumValue + Math.max(0, (positionAtMaximumValue - positionAtMinimumValue) * (1 - ((value - maximumValue) / (minimumValue - maximumValue))));

        if (debug)
        {
            console.log("value: " + value);
            console.log("minimumValue: " + minimumValue);
            console.log("maximumValue: " + maximumValue);
            console.log("position: " + position);
            console.log("positionAtMinimumValue: " + positionAtMinimumValue);
            console.log("positionAtMaximumValue: " + positionAtMaximumValue);
        }
        _blockUpdates = false;
    }
}
