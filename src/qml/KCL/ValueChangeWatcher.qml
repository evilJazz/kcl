import QtQuick 1.0

QtObject {
    id: watcher

    property variant target
    property string propertyName

    signal valueChanged(variant oldValue, variant newValue);

    /* Private stuff */

    onTargetChanged: _updateBinding()
    onPropertyNameChanged: _updateBinding()

    property variant _boundTarget
    property string _boundPropertyName

    property variant _oldValue
    property bool _settingValue;

    function _updateBinding()
    {
        if (_boundTarget !== null && typeof(_boundTarget) !== "undefined" && _boundPropertyName !== "")
        {
            _boundTarget[_boundPropertyName + "Changed"].disconnect(watcher._boundValueChanged);
        }

        _boundTarget = target;
        _boundPropertyName = propertyName;
        _oldValue = null;

        if (_boundTarget !== null && typeof(_boundTarget) !== "undefined" && _boundPropertyName !== "")
        {
            _boundTarget[_boundPropertyName + "Changed"].connect(watcher._boundValueChanged);
        }
    }

    function _boundValueChanged()
    {
        if (_settingValue) return;

        try
        {
            _settingValue = true;

            var newValue = _boundTarget[_boundPropertyName];
            //console.log("target: " + target + " property: " + propertyName + " newValue: " + newValue);
            if (newValue !== _oldValue)
            {
                var oldValue = _oldValue;
                _oldValue = newValue; // set oldValue before descending into signal slots...
                valueChanged(oldValue, newValue);
            }
        }
        finally
        {
            _settingValue = false;
        }
    }
}
