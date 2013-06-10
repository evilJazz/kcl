import QtQuick 1.0

QtObjectWithChildren {
    id: entangler

    property alias leftTarget: left.target
    property alias rightTarget: right.target

    property alias leftPropertyName: left.propertyName
    property alias rightPropertyName: right.propertyName

    ValueChangeWatcher {
        id: left
        onValueChanged: entangler.copyLeftToRight()
    }

    ValueChangeWatcher {
        id: right
        onValueChanged: entangler.copyRightToLeft()
    }

    property alias leftValid: left.valid
    property alias rightValid: right.valid

    property bool valid: leftValid && rightValid

    property bool transferring: true

    Component.onCompleted: transferring = false

    function copyLeftToRight()
    {
        if (entangler.transferring) return;
        if (entangler.valid)
        {
            var value = convertLeftToRight(leftTarget[leftPropertyName]);
            if (rightTarget[rightPropertyName] !== value)
            {
                entangler.transferring = true;
                //console.log("COPIED! " + rightTarget + "." + rightPropertyName + " = " + leftTarget + "." + leftPropertyName + " : " + JSON.stringify(value));
                rightTarget[rightPropertyName] = value;
                entangler.transferring = false;
            }
            else
            {
                //console.log("VALUES EQUAL! " + rightTarget + "." + rightPropertyName + " == " + leftTarget + "." + leftPropertyName + " : " + JSON.stringify(value));
            }
        }
    }

    function copyRightToLeft()
    {
        if (entangler.transferring) return;
        if (entangler.valid)
        {
            var value = convertRightToLeft(rightTarget[rightPropertyName]);
            if (leftTarget[leftPropertyName] !== value)
            {
                entangler.transferring = true;
                //console.log("COPIED! " + leftTarget + "." + leftPropertyName + " = " + rightTarget + "." + rightPropertyName + " : " + JSON.stringify(value));
                leftTarget[leftPropertyName] = value;
                entangler.transferring = false;
            }
            else
            {
                //console.log("VALUES EQUAL! " + leftTarget + "." + leftPropertyName + " == " + rightTarget + "." + rightPropertyName + " : " + JSON.stringify(value));
            }
        }
    }

    function convertRightToLeft(right)
    {
        return right;
    }

    function convertLeftToRight(left)
    {
        return left;
    }
}
