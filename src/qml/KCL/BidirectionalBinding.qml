import QtQuick 1.0

QtObject {
    id: bidiBinding

    property QtObject leftTarget: null
    property QtObject rightTarget: null

    property string leftPropertyName
    property string rightPropertyName

    property bool leftValid: leftTarget !== null ? leftTarget.hasOwnProperty(leftPropertyName) : false
    property bool rightValid: rightTarget !== null ? rightTarget.hasOwnProperty(rightPropertyName) : false
    property bool valid: leftValid && rightValid

    property bool transferring: true
    property variant leftValue: valid ? leftTarget[leftPropertyName] : undefined
    property variant rightValue: valid ? rightTarget[rightPropertyName] : undefined

    Component.onCompleted: transferring = false

    onLeftValueChanged: copyLeftToRight()
    onRightValueChanged: copyRightToLeft()

    function copyLeftToRight()
    {
        if (bidiBinding.transferring) return;
        if (bidiBinding.valid)
        {
            var value = convertLeftToRight(leftTarget[leftPropertyName]);
            if (rightTarget[rightPropertyName] !== value)
            {
                bidiBinding.transferring = true;
                //console.log(rightTarget + "." + rightPropertyName + " = " + leftTarget + "." + leftPropertyName + " : " + JSON.stringify(value));
                rightTarget[rightPropertyName] = value;
                bidiBinding.transferring = false;
            }
        }
    }

    function copyRightToLeft()
    {
        if (bidiBinding.transferring) return;
        if (bidiBinding.valid)
        {
            var value = convertRightToLeft(rightTarget[rightPropertyName]);
            if (leftTarget[leftPropertyName] !== value)
            {
                bidiBinding.transferring = true;
                //console.log(leftTarget + "." + leftPropertyName + " = " + rightTarget + "." + rightPropertyName + " : " + JSON.stringify(value));
                leftTarget[leftPropertyName] = value;
                bidiBinding.transferring = false;
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
