import QtQuick 1.0

QtObjectWithChildren {
    id: entangler

    property alias leftTarget: left.target
    property alias rightTarget: right.target

    property alias leftPropertyName: left.propertyName
    property alias rightPropertyName: right.propertyName

    property alias leftValid: left.valid
    property alias rightValid: right.valid

    property bool valid: leftValid && rightValid

    function copyLeftToRight()
    {
        if (entangler._settingValue) return;
        if (entangler.valid)
        {
            var value = convertLeftToRight(leftTarget[leftPropertyName]);
            if (rightTarget[rightPropertyName] !== value)
            {
                entangler._settingValue = true;
                //console.log("COPIED! " + rightTarget + "." + rightPropertyName + " = " + leftTarget + "." + leftPropertyName + " : " + JSON.stringify(value));
                rightTarget[rightPropertyName] = value;
                entangler._settingValue = false;
            }
            else
            {
                //console.log("VALUES EQUAL! " + rightTarget + "." + rightPropertyName + " == " + leftTarget + "." + leftPropertyName + " : " + JSON.stringify(value));
            }
        }
    }

    function copyRightToLeft()
    {
        if (entangler._settingValue) return;
        if (entangler.valid)
        {
            var value = convertRightToLeft(rightTarget[rightPropertyName]);
            if (leftTarget[leftPropertyName] !== value)
            {
                entangler._settingValue = true;
                //console.log("COPIED! " + leftTarget + "." + leftPropertyName + " = " + rightTarget + "." + rightPropertyName + " : " + JSON.stringify(value));
                leftTarget[leftPropertyName] = value;
                entangler._settingValue = false;
            }
            else
            {
                //console.log("VALUES EQUAL! " + leftTarget + "." + leftPropertyName + " == " + rightTarget + "." + rightPropertyName + " : " + JSON.stringify(value));
            }
        }
    }

    /* Override these methods if you want to customize conversion, e.g. change units or do other calculations. */

    function convertRightToLeft(right)
    {
        return right;
    }

    function convertLeftToRight(left)
    {
        return left;
    }

    /* Private section */

    ValueChangeWatcher {
        id: left
        onValueChanged: entangler.copyLeftToRight()
    }

    ValueChangeWatcher {
        id: right
        onValueChanged: entangler.copyRightToLeft()
    }

    property bool _settingValue: true
    Component.onCompleted: _settingValue = false
}
