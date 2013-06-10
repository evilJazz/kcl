import QtQuick 1.0

QtObject {
    id: watcher

    property QtObject target
    property string propertyName

    property bool valid: _evaluateValid(target, propertyName)

    signal valueChanged(variant oldValue, variant newValue);

    /* Private section */

    onTargetChanged: _updateBinding()
    onPropertyNameChanged: _updateBinding()

    property QtObject _boundTarget
    property string _boundPropertyName

    property variant _oldValue
    property bool _settingValue

    function _updateBinding()
    {
        if (_evaluateValid(_boundTarget, _boundPropertyName))
            _boundTarget[_boundPropertyName + "Changed"].disconnect(watcher._boundValueChanged);

        _boundTarget = target;
        _boundPropertyName = propertyName;
        _oldValue = null;

        if (_evaluateValid(_boundTarget, _boundPropertyName))
        {
            var bindSignal = _boundPropertyName + "Changed";

            if (!_boundTarget.hasOwnProperty(bindSignal))
            {
                console.log("Target " + _boundTarget + " does not implement a signal named " + bindSignal + " for property " + _boundPropertyName);
                return;
            }

            _boundTarget[bindSignal].connect(watcher._boundValueChanged);
        }
    }

    function _evaluateValid(target, propertyName)
    {
        var targetValid = target !== null && typeof(target) !== "undefined";
        var propertyValid = targetValid && target.hasOwnProperty(propertyName);

        if (targetValid && !propertyValid)
            console.log("Target " + target + " does not contain property " + propertyName + "!");

        return propertyValid;
    }

    function _boundValueChanged()
    {
        if (_settingValue) return;

        var newValue = _boundTarget[_boundPropertyName];
        if (newValue !== _oldValue)
        {
            try
            {
                _settingValue = true;
                //console.log("VALUE CHANGED! target: " + target + " property: " + propertyName + " newValue: " + newValue + " oldValue: " + _oldValue);
                var oldValue = _oldValue;
                _oldValue = newValue; // set oldValue before descending into signal slots...
                valueChanged(oldValue, newValue);
            }
            finally
            {
                _settingValue = false;
            }
        }
        else
        {
            //console.log("VALUE UNCHANGED! target: " + target + " property: " + propertyName + " newValue: " + newValue + " oldValue: " + _oldValue);
        }
    }
}
